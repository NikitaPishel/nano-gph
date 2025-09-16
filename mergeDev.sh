#!/bin/bash
set -e  # Exit immediately if a command fails

# Save the current branch (just in case)
CURRENT_BRANCH=$(git rev-parse --abbrev-ref HEAD)

# If not on dev, switch to dev
if [ "$CURRENT_BRANCH" != "dev" ]; then
    git checkout dev
fi

# Checkout master
git checkout master

# Merge dev into master
git merge dev
git push

# Push changes to remote (optional, remove if not needed)
# git push origin master

# Return to dev
git checkout dev

echo "✅ Merged 'dev' into 'master' and returned to 'dev'."
