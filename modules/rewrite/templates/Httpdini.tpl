{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
# BEGIN Gallery 2 Url Rewrite section (GalleryID: {$Httpdini.galleryId})
# Do not edit this section manually. Gallery will overwrite it automatically.

RewriteCond Host: {$Httpdini.host}
RewriteRule {$Httpdini.galleryDirectory}modules/rewrite/data/isapi_rewrite/Rewrite.txt {$Httpdini.galleryDirectory}modules/rewrite/data/isapi_rewrite/Works.txt [O]

{foreach from=$Httpdini.rules item=rule}
{if !empty($rule.conditions)}
{foreach from=$rule.conditions item="condition"}
RewriteCond {$condition.test} {$condition.pattern}{if !empty($condition.flags)}   [{$condition.flags|@implode:","}]{/if}

{/foreach}
{/if}
RewriteRule ([^?]*)(?:\?(.*))? {$rule.substitution}{if !empty($rule.flags)}   [{$rule.flags|@implode:","}]{/if}

{/foreach}

# END Url Rewrite section
