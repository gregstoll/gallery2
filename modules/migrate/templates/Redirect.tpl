{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<p>
  {g->text text="To enable URL Redirection under Apache webserver:"}
</p>
<ol>
  {capture name=mod_rewrite_anchor}
  <a href="http://httpd.apache.org/docs/mod/mod_rewrite.html">mod_rewrite</a>
  {/capture}
  <li>
    {g->text text="Ensure the %s Apache module is enabled."
             arg1=$smarty.capture.mod_rewrite_anchor}
  </li>
  <li>
    {g->text text="Edit or create a file called <tt>.htaccess</tt> in your Gallery1 directory and add the text shown below to the file. Remove G1 rewrite rules from an existing file. Redirects will also work in your Gallery2 directory if G2 is installed in the path where G1 used to be. However, if you also use the G2 URL Rewrite module then activate the G1 redirect rule in that module instead of using the block shown below."}
  </li>
  <li>
    {g->text text="Add the same <tt>.htaccess</tt> block in the Gallery1 albums directory if you also wish to redirect links to image files and album directories. Note that these redirects are not active until the G1 images are actually removed or moved. Omit the !-f line shown below to redirect anyway."}
  </li>
  <li>
    {g->text text="If G2 is installed in the path where G1 used to exist, you will need to remove the G1 file \"gallery_remote2.php\" file before you can use Gallery Remote to upload images to G2."}
  </li>
</ol>

{capture name="baseUrl"}{g->url arg1="controlle=migrate.Redirect"
				forceSessionId=false forceFullUrl=true}{/capture}
<pre class="giDescription">&lt;IfModule mod_rewrite.c&gt;
  RewriteEngine On
  RewriteCond %{ldelim}REQUEST_FILENAME{rdelim} !-f
  RewriteCond %{ldelim}REQUEST_FILENAME{rdelim} !-d
  RewriteCond %{ldelim}REQUEST_FILENAME{rdelim} !gallery_remote2.php
  RewriteRule (.*)$ {$smarty.capture.baseUrl|replace:"controlle=":"controller="|regex_replace:"#^.*?://[^/]*#":""}&amp;g2_path=$1 [QSA]
&lt;/IfModule&gt;</pre>

<p>
  <a href="{g->url arg1="view=migrate.Redirect"}">
    {g->text text="Download .htaccess file"}
  </a>
</p>
