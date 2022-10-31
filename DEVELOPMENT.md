# Branches

* `ahpn-main`: branch to integrate all changes. This branch forks from the main branch. It is used to test and integrate all the customizations on the repo, but it is not
  intended to create pull request toward the upstream repository.
* `ahpn/*` branches: Created from `ahpn-main` to test or develop features. This code is intended for new features, and the code should be cherry picked to a `dev` branch in case it is for a feature to be included on the upstream repo.
* `dev/*` branches: branches that are intended for new code to be integrated on the upstream repository via PR.

# Development environment

Check it at the [Archive of the
Internet](`https://web.archive.org/web/20210224144709/http://codex.gallery2.org/index.php/Gallery2:Developer_Guidelines`)

# Unit testing

Run unit testing by navigating to the following URL:

http://SITE_ADDRESS/lib/tools/phpunit/index.php

To run any test, write the name of the module on the input box and hit `ENTER` (there is no form button).

# Release management

Tools for release management are on folder `lib/tools/release`.
