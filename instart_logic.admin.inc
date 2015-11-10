<?php

/**
 * @file
 * Administrative forms for Instart Logic module.
 */

/**
 * Redirect to purge tab if we have credentials.
 */
function instart_logic_redirect() {
  $username = variable_get('instart_logic_username', '');
  if ($username) {
    drupal_goto('admin/config/services/instart_logic/purge');
  }
  else {
    drupal_goto('admin/config/services/instart_logic/api');
  }
}

/**
 * API access form.
 */
function instart_logic_api_access($form_state) {
  $username = variable_get('instart_logic_username', '');
  $password = variable_get('instart_logic_password', '');
  $customername = variable_get('instart_logic_customername', '');
  $external_domain_name = _instart_logic_get_external_domain();
  $external_base_http = variable_get('instart_logic_external_base_http', 1);
  $external_base_https = variable_get('instart_logic_external_base_https', '');

  $form['credentials'] = array(
    '#type' => 'fieldset',
    '#title' => t('API Access'),
    '#description' => t('Please enter your Instart Logic credentials for API access.
    If you do not have a username contact Instart Logic.'),
  );

  $form['credentials']['instart_logic_username'] = array(
    '#type' => 'textfield',
    '#title' => t('Instart Logic Username'),
    '#default_value' => $username,
    '#required' => TRUE,
  );

  $form['credentials']['instart_logic_password'] = array(
    '#type' => 'password',
    '#title' => t('Instart Logic Password'),
    '#default_value' => $password,
    '#required' => TRUE,
  );

  $form['credentials']['instart_logic_customername'] = array(
    '#type' => 'textfield',
    '#title' => t('Instart Logic Customer Name'),
    '#description' => t('The Customer Name is available in the customer portal
    under the account page.'),
    '#default_value' => $customername,
    '#required' => TRUE,
  );

  $form['domain_name'] = array(
    '#type' => 'fieldset',
    '#title' => t('External Domain Name'),
    '#description' => t('Enter the external domain name of your site.
    Also indicate whether your site is accesible via http and/or https.'),
  );

  $form['domain_name']['instart_logic_external_domain_name'] = array(
    '#type' => 'textfield',
    '#title' => t('External Domain Name'),
    '#description' => t('The external domain name of your site.'),
    '#default_value' => $external_domain_name,
  );

  $form['domain_name']['instart_logic_external_scheme_text'] = array(
    '#type' => 'markup',
    '#markup' => t('Site Uses:'),
  );

  $form['domain_name']['instart_logic_external_base_http'] = array(
    '#type' => 'checkbox',
    '#title' => t('http'),
    '#default_value' => $external_base_http,
  );

  $form['domain_name']['instart_logic_external_base_https'] = array(
    '#type' => 'checkbox',
    '#title' => t('https'),
    '#default_value' => $external_base_https,
  );

  return system_settings_form($form);
}

/**
 * Validate handler for API access form.
 */
function instart_logic_api_access_validate($form, &$form_state) {
  $username = $form_state['values']['instart_logic_username'];
  $password = $form_state['values']['instart_logic_password'];
  $customername = $form_state['values']['instart_logic_customername'];
  $http = $form_state['values']['instart_logic_external_base_http'];
  $https = $form_state['values']['instart_logic_external_base_https'];

  if (!$http && !$https) {
    form_set_error('instart_logic_external_base_http', t('Please select at least one scheme:
    http or https.'));
    return;
  }

  if ($username == ''|| $password == '' || $customername == '') {
    return;
  }

  $api = new Instart($username, $password, $customername);

  try {
    $session_id = _instart_logic_create_session($api);
    $account = $api->getAccount($session_id);
  }
  catch (InstartException $e) {
    drupal_set_message($e->getMessage(), 'error');
    form_set_error('credentials', t('Invalid credentials.
    Please contact Instart Logic.'));
    return;
  }

  $match = _instart_logic_customername_match($account, $customername);

  if (!$match) {
    form_set_error('instart_logic_customername', t('Customer Name does not match the account.'));
  }

  if ($match && $session_id) {
    drupal_set_message(t('Credentials accepted.'));
  }
}

/**
 * Form builder for purge form.
 */
function instart_logic_purge_form($form_state) {
  $form['purge_url'] = array(
    '#type' => 'fieldset',
    '#title' => t('Purge by URL'),
    '#description' => t('Paste one or more URLs to purge. Each in new line.
    <ul>
      <li>One URL per line.</li>
      <li> Absolute URLs only.</li>
      <li> Enter either full URLs or URLs with trailing wildcard.</li>
      <li> The wildcard must be at the end of the pattern.</li>
      </ul>
    Examples:
    http://example.com/blog/topic1,
    http://example.com/blog/*'),
  );

  $form['purge_url']['urls_list'] = array(
    '#type' => 'textarea',
  );

  $form['purge_url']['submit'] = array(
    '#type' => 'button',
    '#value' => t('Purge'),
    '#id' => 'urls',
    '#name' => 'urls',
    '#executes_submit_callback' => 1,
  );

  $form['purge_all'] = array(
    '#type' => 'fieldset',
    '#title' => t('Purge all URLs in domain'),
    '#description' => t('Purge all URLs for the domain from the Instart Logic cache. Use sparingly.'),
  );

  $form['purge_all']['submit'] = array(
    '#type' => 'button',
    '#value' => t('Purge All'),
    '#id' => 'all',
    '#name' => 'all',
    '#executes_submit_callback' => 1,
  );

  return $form;

}

/**
 * Validate handler for purge form.
 */
function instart_logic_purge_form_validate($form, &$form_state) {
  $external_domain_name = _instart_logic_get_external_domain();
  $method = $form_state['triggering_element']['#name'];
  $errors = FALSE;
  $valid_urls = array();
  switch ($method) {
    case 'all':
      $schemes = _instart_logic_get_schemes();
      foreach ($schemes as $scheme) {
        $url = $scheme . '://' . $external_domain_name . '/';
        $url_type = 'PREFIX_LITERAL';
        $valid_urls[$url] = $url_type;
      }
      break;

    case 'urls':
      $values = trim($form_state['values']['urls_list']);
      if (empty($values)) {
        $errors = TRUE;
        form_set_error('urls_list', t('Please input the URLs to purge.'));
      }
      else {
        foreach (explode("\n", $values) as $line) {
          $url = trim($line);
          $url_type = _instart_logic_get_url_type($url);
          if ($url_type == 'PREFIX_LITERAL') {
            $url = rtrim($url, '*');
          }

          if (!valid_url($url, TRUE)) {
            $errors = TRUE;
            form_set_error('urls_list', t('Error: @url is an invalid URL.', array('@url' => $url)));
          }
          else {
            $valid_urls[$url] = $url_type;
          }
        }
      }
      break;

  }
  if (!$errors) {
    cache_set('instart_logic_urls_list', $valid_urls, 'cache');
  }
  else {
    cache_clear_all('instart_logic_urls_list', 'cache');
  }
}

/**
 * Determine the type of a url. Determines whether it is literal or wildcard.
 *
 * @param string $url
 *    The url to verify.
 *
 * @return bool
 *    A boolean value indicating if the url is literal.
 */
function _instart_logic_get_url_type($url) {
  if (substr_count($url, '*') == 1 &&
    strpos($url, '*') == strlen($url) - 1
  ) {
    return 'PREFIX_LITERAL';
  }
  return 'EXACT_LITERAL';
}

/**
 * Purge form submit handler.
 *
 * Implements hook_form_submit().
 */
function instart_logic_purge_form_submit($form, &$form_state) {
  $username = variable_get('instart_logic_username', '');
  $password = variable_get('instart_logic_password', '');
  $customername = variable_get('instart_logic_customername', '');
  $api = new Instart($username, $password, $customername);
  $valid_urls = cache_get('instart_logic_urls_list')->data;
  foreach (array_keys($valid_urls) as $url) {
    drupal_set_message (t('Purging @url', array('@url' => $url)));
  };
  if (!empty($valid_urls)) {
    try {
      $session_id = _instart_logic_get_session($api);
      $api->purgeUrls($valid_urls, $session_id);
    }
    catch (InstartException $e) {
      drupal_set_message($e->getMessage(), 'error');
      return;
    }
    drupal_set_message(t('Successfully submitted the purge request.'));
  }
}
