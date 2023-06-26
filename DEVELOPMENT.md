# Branches

* `dev/*` branches: branches that are intended for new code to be integrated on the upstream repository via PR.

# Development environment

Check it at the [Archive of the
Internet](`https://web.archive.org/web/20210224144709/http://codex.gallery2.org/index.php/Gallery2:Developer_Guidelines`)

# Unit testing

## Setup

You will probably need to install locale files for the following languages: `es` (Spanish), `ca` (Catalan),
`de` (German), `fr` (French) and `pt_BR` (Brazilian Portuguese). Here there are some example for different OS:

```shell
Almalinux 8

dnf install langpacks-es langpacks-ca langpacks-de langpacks-fr langpacks-pt_BR  -y
```

```shell
Debian

See docker config file for an example
```

## Run tests from the browser

Run unit testing by navigating to the following URL:
```text
http://SITE_ADDRESS/lib/tools/phpunit/index.php
```

To run any test, write the name of the module on the input box and hit `ENTER` (there is no form button).

## Launch test from command line

You will need to set two environment variables for the authentication:

* `USERNAME`, use the username you use for login on the admin panel.
* `PASSWORD`, use the password for the user.

This script assumes that you use the same password for the admin user and for the `setup.password`
to access the support area.

For example:
```shell
$ export USERNAME=admin
$ export PASSWORD=myadminpassword

$ grep "setup.password" config.php
...
$gallery->setConfig('setup.password', 'myadminpassword');
...
```

There is a helper script to run tests:
```shell
auto-test.sh <SERVER_URL> <SCOPE>
```

* `SERVER_URL` is the server URL as in `http://host.docker.internal:10000/`.
* `SCOPE` is a regular expression string to restrict testing to classes containing that text
in their class name or test method. If you use an exclamation before a module/class/test
name(s) encapsulated in parenthesis and separated with bars,
this will exclude the matching tests. Use "`:#-#`" to restrict which matching tests are
actually run. You can also specify multiple spans with "`:#-#,#-#,#-#`". Append "`:1by1`"
to run tests one-per-request; automatic refresh stops when a test fails.
```text
    AddCommentControllerTest.testAddComment
    AddCommentControllerTest.testAdd
    AddCommentControllerTest
    comment
    !(comment)
    !(comment|core)
    comment:1-3
    comment:3-
    comment:-5
    comment:1-3,6-8,10-12
    comment:-3,4-
    core:1by1
```

As the test progress, you will see the number of the latest test executed, like this:
```text
testRow1
testRow2
testRow3
...
testRow15
testRow16
...
```
At the end you will see a report available on the name of the scope you selected, with `html`
extension. You can open it on a browser to see more details on the execution.


# Docker

There is a docker compose configuration that will run nginx web server, PHP-FPM server and MySQL server.

In order to start it run:

```shell
docker compose up -d
```

## Initial setup

Point your browser to `http://host.docker.internal:10000/install.php` and follow the instructions.

Use `/gallery2-data` on the Storage setup step. Make sure the directory is writable, since when initially
created it will not allow writing.

```shell
$ docker exec -it gallery2-php8-php-1 bash -c "chmod a+w /gallery2-data"
```

You will find mysql credentials in the `docker-compose.yaml` file. Use `mysql` for the server hostname.


## Configuration

Add the following configuration to `config.php` file:

```php
$gallery->setConfig('baseUri', 'http://host.docker.internal:10000/main.php');
```

This way you will be able to access the web server running on docker, and at the same time, you will be
able to debug the code.

## Locale files

Some unit tests require translation files for the core module.

In order to facilitate the initial setup, there is a script at `lib/tools/phpunit/create-locale.sh` to
create the files in the appropriate location.

In order to use your locale in the docker image, you need to add it to `PHP.Dockerfile`. This is an example
for `ca_ES` locale. Add a line like it using your locale:
```shell
    && sed -i -e 's/# ca_ES ISO-8859-1/ca_ES ISO-8859-1/' /etc/locale.gen \
```

## Copy volumes

You can create a backup copy of the `mysqldata` and `gallery2-data` volumes to start afresh.

There is a `copy-volumes.sh` script that allows you to create a copy of the volume.

In case you would like to reset the state to the one in the vanilla volumes, you just need to copy data from the
vanilla volumes to the new volumes.

You will need to add the volume as an external volume in docker-compose config file:
```yaml
  gallery2-php8_mysqldata-fresh:
    external: true
```

# Release management

Tools for release management are on folder `lib/tools/release`.

# Translation tools

Tools for release management are on folder `lib/tools/po`.

See further details at [Gallery2 Codex](http://codex.galleryproject.org/Gallery2:Localization.html).

# Disable Xdebug on docker

Start docker-composer using a variable to control PHP base image, `PHP_TARGET`:

* do not set it or set it to `base` to use a PHP version with no Xdebug.
* set it to `debug`, to use a PHP version with Xdebug.

Whenever you would like to switch from base to debug, you should build the php service:

Example to activate debug:

```shell
export PHP_TARGET=debug

docker compose down && \
  docker compose build php && \
  docker compose restart php
```

Example to disable debug:

```shell
export PHP_TARGET=base

docker compose down && \
  docker compose build php && \
  docker compose restart php
```
