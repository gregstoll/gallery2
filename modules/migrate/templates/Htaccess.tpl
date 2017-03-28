{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
{capture name="baseUrl"}{g->url arg1="controlle=migrate.Redirect"
				forceSessionId=false forceFullUrl=true}{/capture}
<IfModule mod_rewrite.c>
  RewriteEngine On
  RewriteCond %{ldelim}REQUEST_FILENAME{rdelim} !-f
  RewriteCond %{ldelim}REQUEST_FILENAME{rdelim} !-d
  Rewritecond %{ldelim}REQUEST_FILENAME{rdelim} !gallery_remote2.php
  RewriteRule (.*)$ {$smarty.capture.baseUrl|replace:"controlle=":"controller="|regex_replace:"#^.*?://[^/]*#":""}&g2_path=$1 [QSA]
</IfModule>
