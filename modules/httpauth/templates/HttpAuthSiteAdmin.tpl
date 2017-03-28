{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="HTTP Auth Settings"} </h2>
</div>

{if isset($status.saved)}
  <div class="gbBlock">
    <h2 class="giSuccess"> {g->text text="Settings saved successfully"} </h2>
  </div>
{/if}

{if $HttpAuthSiteAdmin.code & HTTPAUTH_STATUS_MISSING_AUTHORIZATION}
  <div class="gbBlock">
    <h3 class="giWarning"> {g->text text="Missing HTTP Authorization"} </h3>

    <p class="giDescription">
      {g->text text="Gallery can't access HTTP usernames and passwords.  You can still use your web server's authentication.  Troubleshooting documentation is in the %sGallery Codex%s." arg1="<a href=\"http://codex.gallery2.org/index.php/Gallery2:Modules:httpauth\">" arg2="</a>"}
    </p>
  </div>
{/if}

{if $HttpAuthSiteAdmin.code & HTTPAUTH_STATUS_REWRITE_MODULE_DISABLED}
  <div class="gbBlock">
    <h3 class="giWarning"> {g->text text="URL rewrite module disabled"} </h3>

    <p class="giDescription">
      {capture assign="adminPluginsUrl"}{g->url arg1="view=core.SiteAdmin" arg2="subView=core.AdminPlugins" return=true}{/capture}
      {g->text text="We can't fall back on passing HTTP usernames and passwords to Gallery because the URL rewrite module is disabled.  You should activate the URL rewrite module in the %sSite Admin Plugins option%s and choose either Apache mod_rewrite or ISAPI_Rewrite.  Troubleshooting documentation is in the %sGallery Codex%s." arg1="<a href=\"$adminPluginsUrl\">" arg2="</a>" arg3="<a href=\"http://codex.gallery2.org/index.php/Gallery2:Modules:httpauth\">" arg4="</a>"}
    </p>
  </div>
{/if}

{if $HttpAuthSiteAdmin.code & HTTPAUTH_STATUS_BAD_REWRITE_PARSER}
  <div class="gbBlock">
    <h3 class="giWarning"> {g->text text="Bad URL rewrite configuration"} </h3>

    <p class="giDescription">
      {capture assign="adminPluginsUrl"}{g->url arg1="view=core.SiteAdmin" arg2="subView=core.AdminPlugins" return=true}{/capture}
      {g->text text="PHP Path Info rewrite doesn't support the rule to fall back on passing HTTP usernames and passwords to Gallery.  You should uninstall and reinstall the URL rewrite module in the %sSite Admin Plugins option%s and choose either Apache mod_rewrite or ISAPI_Rewrite.  Troubleshooting documentation is in the %sGallery Codex%s." arg1="<a href=\"$adminPluginsUrl\">" arg2="</a>" arg3="<a href=\"http://codex.gallery2.org/index.php/Gallery2:Modules:httpauth\">" arg4="</a>"}
    </p>
  </div>
{/if}

{if $HttpAuthSiteAdmin.code & HTTPAUTH_STATUS_AUTHORIZATION_RULE_DISABLED}
  <div class="gbBlock">
    <h3 class="giWarning"> {g->text text="'Authorization Header' rule disabled"} </h3>

    <p class="giDescription">
      {capture assign="adminRewriteUrl"}{g->url arg1="view=core.SiteAdmin" arg2="subView=rewrite.AdminRewrite" return=true}{/capture}
      {g->text text="The URL rewrite rule to fall back on passing HTTP usernames and passwords to Gallery is disabled.  You should activate the HTTP auth 'Authorization Header' rule in the %sSite Admin URL Rewrite option%s.  Troubleshooting documentation is in the %sGallery Codex%s." arg1="<a href=\"$adminRewriteUrl\">" arg2="</a>" arg3="<a href=\"http://codex.gallery2.org/index.php/Gallery2:Modules:httpauth\">" arg4="</a>"}
    </p>
  </div>
{/if}

{if $HttpAuthSiteAdmin.code & HTTPAUTH_STATUS_ERROR_UNKNOWN}
  <div class="gbBlock">
    <h3 class="giWarning"> {g->text text="Unknown Cause"} </h3>

    <p class="giDescription">
      {g->text text="Gallery can't access HTTP usernames and passwords and automated checks failed to find a cause.  Troubleshooting documentation is in the %sGallery Codex%s." arg1="<a href=\"http://codex.gallery2.org/index.php/Gallery2:Modules:httpauth\">" arg2="</a>"}
    </p>
  </div>
{/if}

<div class="gbBlock">
  <p class="giDescription">
    {g->text text="Gallery can handle HTTP Basic authentication directly."}
  </p>

  <label for="cbHttpAuthPluginInput"> {g->text text="Use HTTP Authentication:"} </label>
  <input id="cbHttpAuthPluginInput" name="{g->formVar var="form[httpAuthPlugin]"}" type="checkbox"{if !empty($form.httpAuthPlugin)} checked="checked"{/if} onclick="BlockToggle('cbAuthName')"/>
</div>

<div class="gbBlock" id="cbAuthName"{if empty($form.httpAuthPlugin)} style="display: none"{/if}>
  <p class="giDescription">
    {g->text text="Gallery will prompt you to login with HTTP authentication when permission is denied.  HTTP authentication sends your client a realm to which your username belongs.  It's safe to leave the realm blank."}
  </p>
  {g->text text="HTTP Authentication Realm:"}
  <input name="{g->formVar var="form[authName]"}" type="text" value="{$form.authName}"/>
</div>

<div class="gbBlock">
  <p class="giDescription">
    {g->text text="Your web server may offer more comprehensive and more secure authentication.  If you configured your web server to authenticate requests to Gallery, you may choose to trust the username it reports in the REMOTE_USER environment variable."}
  </p>

  <label for="cbServerAuthPluginInput"> {g->text text="Use Web Server Authentication:"} </label>
  <input id="cbServerAuthPluginInput" name="{g->formVar var="form[serverAuthPlugin]"}" type="checkbox"{if !empty($form.serverAuthPlugin)} checked="checked"{/if}/>
</div>

<div class="gbBlock">
  <p class="giDescription">
    {g->text text="By default HTTP authentication is only enabled for specific modules."}
  </p>

  <label for="cbUseGlobally"> {g->text text="Use the authentication plugins for all modules:"} </label>
  <input id="cbUseGlobally" name="{g->formVar var="form[useGlobally]"}" type="checkbox"{if !empty($form.useGlobally)} checked="checked"{/if}/>
</div>

<div class="gbBlock">
  <p class="giDescription">
    {g->text text="You may wish to trust only HTTP authentication types or HTTP usernames which match specified regular expressions - or HTTP usernames may not match your Gallery usernames; for instance if using %s authentication, the REMOTE_USER environment variable may be username@REALM.TLD.  In these cases, you may use regular expressions to filter authentication types and usernames." arg1="<a href=\"http://modauthkerb.sourceforge.net/\"> Kerberos </a>"}
  </p>

  <label for="cbRegexAuthPluginInput"> {g->text text="Use Regular Expressions:"} </label>
  <input id="cbRegexAuthPluginInput" name="{g->formVar var="form[regexAuthPlugin]"}" type="checkbox"{if !empty($form.regexAuthPlugin)} checked="checked"{/if} onclick="BlockToggle('cbAuthtypeRegex'); BlockToggle('cbUsernameRegex')"/>
</div>

<div class="gbBlock" id="cbAuthtypeRegex"{if empty($form.regexAuthPlugin)} style="display: none"{/if}>
  <p class="giDescription">
    {g->text text="Specify here a regular expression which the authentication type must match for authentication to proceed; for instance /Negotiate/"}
  </p>

  {g->text text="Authentication Type Pattern:"}
  <input name="{g->formVar var="form[authtypePattern]"}" type="text" value="{$form.authtypePattern}"/>

  {if !empty($form.error.authtype.regex.invalid)}
    <div class="giError"> {g->text text="You must enter a valid regular expression"} </div>
    <div class="giError"> {$status.authtypeRegexMessage} </div>
  {/if}
</div>

<div class="gbBlock" id="cbUsernameRegex" {if empty($form.regexAuthPlugin)} style="display: none"{/if}>
  <p class="giDescription">
    {g->text text="Specify here a regular expression which the username must match for authentication to proceed and a string with which to replace it.  See PHP %s documentation for more information." arg1="<a href=\"http://php.net/preg_replace\"> preg_replace </a>"}
  </p>

  {g->text text="Username Pattern:"}
  <input name="{g->formVar var="form[usernamePattern]"}" type="text" value="{$form.usernamePattern}"/>

  {g->text text="Username Replacement:"}
  <input name="{g->formVar var="form[usernameReplace]"}" type="text" value="{$form.usernameReplace}"/>

  {if !empty($form.error.username.regex.invalid)}
    <div class="giError"> {g->text text="You must enter a valid regular expression"} </div>
    <div class="giError"> {$status.usernameRegexMessage} </div>
  {/if}
</div>

<div class="gbBlock gcBackground1">
  <input class="inputTypeSubmit" name="{g->formVar var="form[action][save]"}" type="submit" value="{g->text text="Save"}"/>
  <input class="inputTypeSubmit" name="{g->formVar var="form[action][reset]"}" type="submit" value="{g->text text="Reset"}"/>
</div>
