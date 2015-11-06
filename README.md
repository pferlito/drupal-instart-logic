# Instart Logic

Project Page:
http://drupal.org/project/instart_logic

## CONTENTS OF THIS FILE

 * Introduction
 * Requirements
 * Installation
 * Configuration
 * Troubleshooting
 * FAQ
 * Maintainers
 * Credits


### Introduction

About Instart Logic:
Instart Logic provides the first cloud application delivery service in the world
to extend beyond the limited backend-only cloud architectures of the past with
an innovative, intelligent client-cloud platform delivered as a service.
We call this new approach Software-defined Application Delivery since it’s based
on a software-defined architecture rather than on hardware and network scale.

The unique client-cloud platform provides true end-to-end control of the
application delivery path from client to cloud to origin. This enables radical
new approaches to application and content delivery performance, security,
analytics, and more.

The service requires no code changes or SDKs to integrate and works for all
existing browsers without any plug-ins or other add-ons. It’s totally
transparent to users and the backend infrastructure and requires only simple DNS
changes to use.

### Instart Logic Module Features:

1. Automated Purging.
Content can be automatically purged when updated/created.
This is done through the Cache Expiration module.

2. Manual purging.
You can purge individual URLs, URLs in bulk using wildcards or purge all of the
content.

### Requirements

The Instart Logic module requires the [Cache Expiration](https://www.drupal.org/project/expire) module
.


### Installation

Install as you would normally install a contributed drupal module. See:
   https://drupal.org/documentation/install/modules-themes/modules-7
   for further information.


### Configuration

1. Go to Configuration -> Web Services ->Instart Logic Configuration -> API Access,
   url /admin/config/services/instart_logic/api and enter your credentials.
   If your site is reachable under a different domain enter it in the External
   Domain Name field.

2. Go to the Purge tab to purge URLs. You can enter URLs manually or purge all
   URLs at once. A wildcard (*) is accepted at the end of the URLs for bulk
   purging.

   Examples:
   http://example.com/blog/topic1 -- purge a single URL
   http://example.com/blog/* -- purge all URLs that begin with /blog

3. If you are using the Cache Expiration module see that module's configuration
   page to turn on automatic purging. You can enable this by visiting
   admin/config/system/expire.  You should see Instart Logic in the list of
   modulesthat support external expiration. Make sure you select "External
   expiration", and also ensure you untick "Include base URL in expires".


### Troubleshooting

If you encounter trouble with this module submit an issue in the issue queue.

### FAQ


### Maintainers

Paul Ferlito - Ferlito/van der Wyk

Github - [pferlito](https://github.com/pferlito)

Drupal -  [pferlito](https://www.drupal.org/user/511172)

Credits
-------

This project is sponsored by [Instart Logic Inc.](https://www.instartlogic.com)

Development by [Ferlito/van der Wyk](http://www.pfvdw.com)
