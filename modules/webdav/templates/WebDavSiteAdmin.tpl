{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="WebDAV Settings"} </h2>
</div>

{if !$WebDavSiteAdmin.code}
  <div class="gbBlock">
    <h3 class="giSuccess"> {g->text text="Configuration checked successfully"} </h3>

    <p class="giDescription">
      {g->text text="Most WebDAV clients will successfully connect.  Documentation on mounting Gallery with WebDAV is in the %sGallery Codex%s." arg1="<a href=\"http://codex.gallery2.org/index.php/Gallery2:Modules:webdav:user\">" arg2="</a>"}
    </p>

    <p class="giDescription">
      {g->text text="The URL to connect to Gallery with WebDAV is:"} <a href="{g->url arg1="controller=webdav.WebDav" forceFullUrl=true forceSessionId=false useAuthToken=false}"> {g->url arg1="controller=webdav.WebDav" forceFullUrl=true forceSessionId=false useAuthToken=false} </a>
    </p>
  </div>
{/if}

{if $WebDavSiteAdmin.code & WEBDAV_STATUS_NO_XML_PARSER}
  <div class="gbBlock">
    <h3 class="giError"> {g->text text="PHP has no XML support"} </h3>

    <p class="giDescription">
      {g->text text="You can't connect with WebDAV because PHP has no XML support on this server.  Troubleshooting documentation is in the %sGallery Codex%s." arg1="<a href=\"http://codex.gallery2.org/index.php/Gallery2:Modules:webdav:admin\">" arg2="</a>"}
    </p>
  </div>
{/if}

{if $WebDavSiteAdmin.code & WEBDAV_STATUS_METHOD_NOT_HANDLED}
  <div class="gbBlock">
    <h3 class="giError"> {g->text text="WebDAV requests not handled"} </h3>

    <p class="giDescription">
      {g->text text="You can't connect with WebDAV because this server doesn't pass WebDAV requests to Gallery.  Troubleshooting documentation is in the %sGallery Codex%s." arg1="<a href=\"http://codex.gallery2.org/index.php/Gallery2:Modules:webdav:admin\">" arg2="</a>"}
    </p>
  </div>
{/if}

{if $WebDavSiteAdmin.code & WEBDAV_STATUS_HTTPAUTH_MODULE_DISABLED}
  <div class="gbBlock">
    <h3 class="giWarning"> {g->text text="HTTP auth module disabled"} </h3>

    <p class="giDescription">
      {capture assign="adminPluginsUrl"}{g->url arg1="view=core.SiteAdmin" arg2="subView=core.AdminPlugins" return=true}{/capture}
      {g->text text="You can connect with WebDAV anonymously, but you can't do anything which requires you to login because the HTTP auth module is disabled.  You should activate the HTTP auth module in the %sSite Admin Plugins option%s.  Troubleshooting documentation is in the %sGallery Codex%s." arg1="<a href=\"$adminPluginsUrl\">" arg2="</a>" arg3="<a href=\"http://codex.gallery2.org/index.php/Gallery2:Modules:webdav:admin\">" arg4="</a>"}
    </p>
  </div>
{elseif $WebDavSiteAdmin.code & WEBDAV_STATUS_HTTPAUTH_AUTH_PLUGINS_DISABLED}
  <div class="gbBlock">
    <h3 class="giWarning"> {g->text text="HTTP auth plugin disabled"} </h3>

    <p class="giDescription">
      {g->text text="You can connect with WebDAV anonymously, but you can't do anything which requires you to login because neither HTTP authentication nor server authentication are enabled in the HTTP auth module.  You should activate HTTP authentication in the settings of the HTTP auth module."}
    </p>
  </div>

{/if}

{if $WebDavSiteAdmin.code & WEBDAV_STATUS_CONNECT_RULE_DISABLED}
  <div class="gbBlock">
    <h3 class="giWarning"> {g->text text="'Connect to WebDAV' rule disabled"} </h3>

    <p class="giDescription">
      {capture assign="adminRewriteUrl"}{g->url arg1="view=core.SiteAdmin" arg2="subView=rewrite.AdminRewrite" return=true}{/capture}
      {g->text text="Most WebDAV clients will fail to connect because the URL rewrite rule to generate short WebDAV URLs is disabled.  You should activate the 'Connect to WebDAV' rule in the %sSite Admin URL Rewrite option%s.  Troubleshooting documentation is in the %sGallery Codex%s." arg1="<a href=\"$adminRewriteUrl\">" arg2="</a>" arg3="<a href=\"http://codex.gallery2.org/index.php/Gallery2:Modules:webdav:admin\">" arg4="</a>"}
    </p>

    <p class="giDescription">
      {g->text text="The URL to connect to Gallery with WebDAV is:"} <a href="{g->url arg1="controller=webdav.WebDav" forceFullUrl=true forceSessionId=false useAuthToken=false}"> {g->url arg1="controller=webdav.WebDav" forceFullUrl=true forceSessionId=false useAuthToken=false} </a>
    </p>
  </div>
{/if}

{if $WebDavSiteAdmin.code & WEBDAV_STATUS_MISSING_DAV_HEADERS}
  <div class="gbBlock">
    <h3 class="giWarning"> {g->text text="Missing DAV headers"} </h3>

    <p class="giDescription">
      {g->text text="Some WebDAV clients, e.g. Mac OS X WebDAVFS, will fail to connect because OPTIONS responses are missing DAV headers.  Troubleshooting documentation is in the %sGallery Codex%s." arg1="<a href=\"http://codex.gallery2.org/index.php/Gallery2:Modules:webdav:admin\">" arg2="</a>"}
    </p>
  </div>
{/if}

{if $WebDavSiteAdmin.code & WEBDAV_STATUS_ALTERNATIVE_URL_HEADERS}
  <div class="gbBlock">
    <h3 class="giWarning"> {g->text text="Alternative URL missing DAV headers"} </h3>

    <p class="giDescription">
      {g->text text="Because OPTIONS responses are missing DAV headers, we try to fall back on an alternative URL, but alternative URL responses are also missing DAV headers.  Troubleshooting documentation is in the %sGallery Codex%s." arg1="<a href=\"http://codex.gallery2.org/index.php/Gallery2:Modules:webdav:admin\">" arg2="</a>"}
    </p>
  </div>
{/if}

{if $WebDavSiteAdmin.code & WEBDAV_STATUS_REWRITE_MODULE_DISABLED}
  <div class="gbBlock">
    <h3 class="giWarning"> {g->text text="URL rewrite module disabled"} </h3>

    <p class="giDescription">
      {capture assign="adminPluginsUrl"}{g->url arg1="view=core.SiteAdmin" arg2="subView=core.AdminPlugins" return=true}{/capture}
      {g->text text="Most WebDAV clients will fail to connect because the URL rewrite module is disabled.  You should activate the URL rewrite module in the %sSite Admin Plugins option%s and choose either Apache mod_rewrite or ISAPI_Rewrite.  Troubleshooting documentation is in the %sGallery Codex%s." arg1="<a href=\"$adminPluginsUrl\">" arg2="</a>" arg3="<a href=\"http://codex.gallery2.org/index.php/Gallery2:Modules:webdav:admin\">" arg4="</a>"}
    </p>
  </div>
{/if}

{if $WebDavSiteAdmin.code & WEBDAV_STATUS_BAD_REWRITE_PARSER}
  <div class="gbBlock">
    <h3 class="giWarning"> {g->text text="Bad URL rewrite configuration"} </h3>

    <p class="giDescription">
      {capture assign="adminPluginsUrl"}{g->url arg1="view=core.SiteAdmin" arg2="subView=core.AdminPlugins" return=true}{/capture}
      {g->text text="PHP PathInfo rewrite doesn't support the rule to fall back on an alternative URL.  You should uninstall and reinstall the URL rewrite module in the %sSite Admin Plugins option%s and choose either Apache mod_rewrite or ISAPI_Rewrite.  Troubleshooting information is in the %sGallery Codex%s." arg1="<a href=\"$adminPluginsUrl\">" arg2="</a>" arg3="<a href=\"http://codex.gallery2.org/index.php/Gallery2:Modules:webdav:admin\">" arg4="</a>"}
    </p>
  </div>
{/if}

{if $WebDavSiteAdmin.code & WEBDAV_STATUS_OPTIONS_RULE_DISABLED}
  <div class="gbBlock">
    <h3 class="giWarning"> {g->text text="'OPTIONS Requests' rule disabled"} </h3>

    <p class="giDescription">
      {capture assign="adminRewriteUrl"}{g->url arg1="view=core.SiteAdmin" arg2="subView=rewrite.AdminRewrite" return=true}{/capture}
      {g->text text="The URL rewrite rule to fall back on an alternative URL is disabled.  You should activate the WebDAV 'OPTIONS Requests' rule in the %sSite Admin URL Rewrite option%s.  Troubleshooting documentation is in the %sGallery Codex%s." arg1="<a href=\"$adminRewriteUrl\">" arg2="</a>" arg3="<a href=\"http://codex.gallery2.org/index.php/Gallery2:Modules:webdav:admin\">" arg4="</a>"}
    </p>
  </div>
{/if}

{if $WebDavSiteAdmin.code & WEBDAV_STATUS_ERROR_UNKNOWN}
  <div class="gbBlock">
    <h3 class="giWarning"> {g->text text="Unknown Cause"} </h3>

    <p class="giDescription">
      {g->text text="Some WebDAV clients, e.g. Mac OS X WebDAVFS, will fail to connect and automated checks failed to find a cause.  Troubleshooting documentation is in the %sGallery Codex%s." arg1="<a href=\"http://codex.gallery2.org/index.php/Gallery2:Modules:webdav:admin\">" arg2="</a>"}
    </p>
  </div>
{/if}
