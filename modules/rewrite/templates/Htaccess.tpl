{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
# BEGIN Url Rewrite section
# (Automatically generated.  Do not edit this section)
<IfModule mod_rewrite.c>
{if $Htaccess.needOptions}
    Options +FollowSymlinks
{/if}
    RewriteEngine On

    RewriteBase {$Htaccess.rewriteBase}

    RewriteCond %{ldelim}REQUEST_FILENAME{rdelim} -f [OR]
    RewriteCond %{ldelim}REQUEST_FILENAME{rdelim} -d [OR]
    RewriteCond %{ldelim}REQUEST_FILENAME{rdelim} gallery\_remote2\.php
    RewriteCond %{ldelim}REQUEST_URI{rdelim} !{$Htaccess.matchBaseFile}$
    RewriteRule .   -   [L]

{foreach from=$Htaccess.rules item=rule}
{if !empty($rule.conditions)}
{foreach from=$rule.conditions item="condition"}
    RewriteCond %{ldelim}{$condition.test}{rdelim} {$condition.pattern}{if !empty($condition.flags)}   [{$condition.flags|@implode:","}]{/if}

{/foreach}
{/if}
    RewriteRule .   {$rule.substitution}{if !empty($rule.flags)}   [{$rule.flags|@implode:","}]{/if}

{/foreach}
</IfModule>

# END Url Rewrite section

