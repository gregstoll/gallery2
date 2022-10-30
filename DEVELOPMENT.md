# Branches

* `ahpn-main`: branch to integrate all changes. This branch forks from the main branch. It is used to test and integrate all the customizations on the repo, but it is not
  intended to create pull request toward the upstream repository.
* `ahpn/*` branches: Created from `ahpn-main` to test or develop features. This code is intended for new features, and the code should be cherry picked to a `dev` branch in case it is for a feature to be included on the upstream repo.
* `dev/*` branches: branches that are intended for new code to be integrated on the upstream repository via PR.
