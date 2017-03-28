{*
 * $Revision: 17678 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="General Settings"} </h2>
</div>

{if !empty($status)}
<div class="gbBlock"><h2 class="giSuccess">
  {if isset($status.saved)}
    {g->text text="Settings saved successfully"} <br/>
  {/if}
  {if isset($status.addedDir)}
    {g->text text="Added local upload directory successfully"}
  {/if}
  {if isset($status.removedDir)}
    {g->text text="Removed local upload directory successfully"}
  {/if}
  {if isset($status.emailTestSuccess)}
    {g->text text="Test email sent successfully"}
  {/if}
</h2></div>
{/if}
{if !empty($form.error)}
<div class="gbBlock"><h2 class="giError">
  {g->text text="There was a problem processing your request, see below for details."}
</h2></div>
{/if}
{if isset($status.emailTestError) or isset($form.emailTestError)}
<div class="gbBlock"><h2 class="giError">
  {g->text text="Error sending test email, see below for details."}
</h2></div>
{/if}

<div class="gbBlock">
  <h3> {g->text text="Date Formats"} </h3>

  <p class="giDescription">
    {g->text text="Specify how dates and times are displayed by Gallery.  See %sphp.net%s for details of how to enter a format string.  Note that the display of some tokens varies according to the active language." arg1="<a href=\"http://php.net/strftime\">" arg2="</a>"}
  </p>

  <table class="gbDataTable"><tr>
    <th>
      {g->text text="Style"}
    </th><th>
      {g->text text="Format"}
    </th><th>
      {g->text text="Sample"}
    </th></tr><tr><td>
      {g->text text="Date"}
    </td><td>
      <input type="text" size="12"
       name="{g->formVar var="form[format][date]"}" value="{$form.format.date}"/>
    </td><td>
      {g->date format=$form.format.date}
    </td></tr><tr><td>
      {g->text text="Time"}
    </td><td>
      <input type="text" size="12"
       name="{g->formVar var="form[format][time]"}" value="{$form.format.time}"/>
    </td><td>
      {g->date format=$form.format.time}
    </td></tr><tr><td>
      {g->text text="Date/Time"}
    </td><td>
      <input type="text" size="12"
       name="{g->formVar var="form[format][datetime]"}" value="{$form.format.datetime}"/>
    </td><td>
      {g->date format=$form.format.datetime}
    </td></tr>
  </table>
</div>

{if isset($AdminCore.can.setPermissions)}
<div class="gbBlock">
  <h3> {g->text text="Filesystem Permissions"} </h3>

  <p class="giDescription">
    {g->text text="Specify the default permissions for files and directories that Gallery creates. This doesn't apply to files/directories that Gallery has already created."}
  </p>

  <table class="gbDataTable"><tr>
    <td>
      {g->text text="New directories"}
    </td><td>
      <select name="{g->formVar var="form[permissions][directory]"}">
	{html_options values=$AdminCore.permissionsDirectoryList
	 selected=$form.permissions.directory output=$AdminCore.permissionsDirectoryList}
      </select>
    </td>
  </tr><tr>
    <td>
      {g->text text="New files"}
    </td><td>
      <select name="{g->formVar var="form[permissions][file]"}">
	{html_options values=$AdminCore.permissionsFileList
	 selected=$form.permissions.file output=$AdminCore.permissionsFileList}
      </select>
    </td>
  </tr></table>
</div>
{/if}

<div class="gbBlock">
  <h3> {g->text text="Session Settings"} </h3>

  <table class="gbDataTable"><tr>
    <td>
      {g->text text="Session Lifetime"}
    </td><td>
      <select name="{g->formVar var="form[session][lifetime]"}">
	{html_options options=$AdminCore.sessionTimeList selected=$form.session.lifetime}
      </select>
    </td>
  </tr><tr>
    <td>
      {g->text text="Inactivity Timeout"}
    </td><td>
      <select name="{g->formVar var="form[session][inactivityTimeout]"}">
	{html_options options=$AdminCore.sessionTimeList selected=$form.session.inactivityTimeout}
      </select>
    </td>
  </tr><tr>
    <td>
      {g->text text="Site Administration Session Timeout"}
    </td><td>
      <select name="{g->formVar var="form[session][siteAdministrationTimeout]"}">
	{html_options options=$AdminCore.sessionTimeList selected=$form.session.siteAdministrationTimeout}
      </select>
    </td>
  </tr></table>
</div>

<div class="gbBlock">
  <h3> {g->text text="Embedded Markup"} </h3>

  <p class="giDescription">
    {g->text text="What kind of markup should we allow in user-entered fields?  For security reasons we do not recommend that you allow raw HTML.  BBCode is a special kind of markup that is secure and allows for simple text formatting like bold, italics, lists, images and urls."}
  </p>

  <table class="gbDataTable"><tr>
    <td>
      {g->text text="Markup"}
    </td><td>
      <select name="{g->formVar var="form[misc][markup]"}">
	{html_options options=$AdminCore.embeddedMarkupList selected=$form.misc.markup}
      </select>
    </td>
  </tr></table>
</div>

<div class="gbBlock">
  <h3> {g->text text="Email"} </h3>

  <p class="giDescription">
    {g->text text="By default Gallery uses PHP's built in mail function to send email which requires no configuration.  To use a smtp/mail server enter the information below, including authentication information if required.  Optionally add :port after the server name to use a non-default port."}
  </p>

  <table class="gbDataTable"><tr>
    <td>
      {g->text text="Server"}
    </td><td>
      <input type="text" size="20"
       name="{g->formVar var="form[smtp][host]"}" value="{$form.smtp.host}"/>
    </td>
  </tr><tr>
    <td>
      {g->text text="Username"}
    </td><td>
      <input type="text" size="20"
       name="{g->formVar var="form[smtp][username]"}" value="{$form.smtp.username}"/>
    </td>
  </tr><tr>
    <td>
      {g->text text="Password"}
    </td><td>
      <input type="password" size="20"
       name="{g->formVar var="form[smtp][password]"}" value="{$form.smtp.password}"/>
    </td>
  </tr><tr>
    <td>
      {g->text text="SMTP From Address"}
    </td><td>
      <input type="text" size="20"
       name="{g->formVar var="form[smtp][from]"}" value="{$form.smtp.from}"/>
      {if isset($form.error.smtp.invalidFrom)}
      <div class="giError">
	{g->text text="Invalid email address"}
      </div>
      {/if}
    </td>
  </tr></table>

  <p class="giDescription">
    {g->text text="Send a test email to verify your settings are correct (whether using PHP mail or SMTP settings above).  Below enter a recipient email address for a test message."}
  </p>
  <p>
    <input type="text" size="30"
     name="{g->formVar var="form[emailTest][to]"}" value="{$form.emailTest.to}"/>
    &nbsp;
    <input type="submit" class="inputTypeSubmit"
     name="{g->formVar var="form[action][emailTest]"}" value="{g->text text="Send Email"}"/>
  </p>
  {if isset($form.emailTestError.invalidTo)}
  <div class="giError">
    {g->text text="Invalid email address"}
  </div>
  {/if}
  {if isset($status.emailTestError)}
    <h4> {g->text text="Email Test Error"} </h4>
    <div class="gcBackground1 gcBorder2"
     style="border-width: 1px; border-style: dotted; padding: 4px">
      {$status.emailTestError}
      <pre>{$status.emailTestDebug}</pre>
    </div>
  {/if}
</div>

<div class="gbBlock">
  <h3>
    {g->text text="Cookies"}
    <span id="AdminCore_cookie-toggle"
     class="giBlockToggle gcBackground1 gcBorder2" style="border-width: 1px"
     onclick="BlockToggle('AdminCore_cookieDetails', 'AdminCore_cookie-toggle')">+</span>
  </h3>
  <p class="giDescription">
    {g->text text="When Gallery is embedded in another application (portal, CMS, forum, etc.), then you have the choice between two options. Everyone else does not have to care about the cookie settings. Read on for more details."}
  </p>
  <p id="AdminCore_cookieDetails" class="gcBorder2"
   style="display: none; border-width: 1px; border-style: dotted; padding: 4px">
    {g->text text="If your Gallery is embedded and you leave the following fields empty, then all DownloadItem links (the URLs of the images and other items) in the embedded Gallery have <b>an appended GALLERYSID string</b> in the URL which is <b>a minor security risk</b> when your Gallery users start copy'n'pasting image URLs in forums, guestbooks, etc. The alternative is to set the <b>cookie path</b>. Gallery will then <b>not append the GALLERYSID to the embedded DownloadItem URLs</b>. E.g. when Gallery is reachable at http://www.example.com/application/gallery2/ and the embedding application is at http://www.example.com/application/, then you have to compare the path /application/gallery2/ with /application/. The cookie path is the part of the paths that is equal, in this case it is '/application/'. Most often it is just '/'."}  <br/>
    {g->text text="The <b>cookie domain</b> is also only needed for embedded Gallery installs and only if you want to get rid of the GALLERYSID string in the embedded DownloadItem URLs. <b>In most cases, the cookie domain can be left blank.</b> Set it only, if Gallery and the embedding application are only reachable with <b>different subdomains</b>. E.g. when Gallery is at http://photos.example.com/ and the application is at http://www.example.com/, then you have to set the cookie domain example.com (the part of the host string that is common to both, Gallery and the embedding application)."} <br/>
    {g->text text="Once you change the cookie settings, <b>all registered users</b> of your Gallery will <b>have to clear their browser cookie cache</b>. If they do not, they will experience login / logout / lost session problems."}
  </p>

  <table class="gbDataTable"><tr>
    <td>
      {g->text text="Path"}
    </td><td>
      <input type="text" size="20"
       name="{g->formVar var="form[cookie][path]"}" value="{$form.cookie.path}"/>
      {if isset($form.error.cookie.invalidPath)}
      <div class="giError">
	{g->text text="Invalid cookie path"} <br/>
      </div>
      {/if}
    </td>
  </tr><tr>
    <td>
      {g->text text="Domain"}
    </td><td>
      <input type="text" size="20"
       name="{g->formVar var="form[cookie][domain]"}" value="{$form.cookie.domain}"/>
      {if isset($form.error.cookie.invalidDomain)}
      <div class="giError">
	{g->text text="Invalid cookie domain"} <br/>
      </div>
      {/if}
    </td>
  </tr></table>
</div>

<div class="gbBlock">
  <h3> {g->text text="Locking System"} </h3>

  <p class="giDescription">
    {g->text text="Gallery uses a system of locks to prevent simultaneous changes from interfering with each other.  There are two types of locking, each with its advantages and disadvantages.  <b>File</b> based locking is fast and efficient, but won't work on NFS filesystems and will be unreliable on Windows.  <b>Database</b> locking is slower but is more reliable.  If you are unsure which to choose, we recommend using file locking.  If you're getting many lock timeouts, you can try switching to database locking instead.  It's ok to switch back and forth."}
  </p>

  <table class="gbDataTable"><tr>
    <td>
      {g->text text="Lock system"}
    </td><td>
      <select name="{g->formVar var="form[lock][system]"}">
	{html_options options=$AdminCore.lockSystemList selected=$form.lock.system}
      </select>
    </td>
  </tr></table>
</div>

{if isset($AdminCore.can.tweakSystemProcesses)}
<div class="gbBlock">
  <h3> {g->text text="Helper Processes"} </h3>
  <p class="giDescription">
    {g->text text="Some Gallery modules will use programs on your server to do image processing, archiving and other operations.  These programs can be very computationally intensive and can impact the overall performance of a shared web server.  You can make these programs run at a lower priority so that they play nice.  If you're in a shared hosting environment and your web host is complaining, try setting your priority low."}
  </p>

  <table class="gbDataTable">
    <tr>
      <td> {g->text text="Priority"} </td>
      <td>
	<select name="{g->formVar var="form[exec][beNice]"}">
	  {html_options options=$AdminCore.beNiceList selected=$form.exec.beNice}
	</select>
      </td>
    </tr>
  </table>
</div>
{/if}

<div class="gbBlock gcBackground1">
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][save]"}" value="{g->text text="Save"}"/>
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][reset]"}" value="{g->text text="Reset"}"/>
</div>
