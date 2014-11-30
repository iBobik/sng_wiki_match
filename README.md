sng_wiki_match
==============

SNG Hackathon project Nájdi môjho tvorcu! http://www.hackathon.io/najdi-mojho

Installation
------------
1. Prepare webserver+PHP+MySQL stack regarding http://drupal.org/requirements. (You can use other supported database engines, but you will not be able to use dump from this repository.)
2. (optional) Your dev workflow will be much smoother if you use Drush: https://github.com/drush-ops/drush
3. Create DB and import there app.mysql.sql
4. Connect Drupal to created DB (run in browser app/install.php)

Development
-----------
1. Create changes in Drupal
   - Change config by GUI
   - Change code in sites/default (you should not change code anywhere else, see https://www.drupal.org/best-practices).
   - Drupal config is handled by Features (see https://www.drupal.org/project/features) to support config versioning, so if you made changes in GUI you should "recreate features".
2. Commit changes to GIT (and eventually fork and push changes there)
3. Repeat 1-2 until you are happy.

If you are likely to continue development, contact me, I will guide you.
