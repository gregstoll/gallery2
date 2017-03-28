{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div id="gsContent" class="gcBorder1" style="border-width: 1px 0 0 1px">
  <div class="gbBlock gcBackground1">
    <h2>
      {if isset($ErrorPage.code.obsoleteData)}
	{g->text text="Edit Conflict!"}
      {elseif isset($ErrorPage.code.securityViolation)}
	{g->text text="Security Violation"}
      {elseif isset($ErrorPage.code.storageFailure)}
	{g->text text="Database Error"}
      {elseif isset($ErrorPage.code.platformFailure)}
	{g->text text="Platform Error"}
      {elseif isset($ErrorPage.code.requestAuthenticationFailure)}
	{g->text text="Authentication Failure"}
      {else}
	{g->text text="Error"}
      {/if}
    </h2>
  </div>

  <div class="gbBlock">
    {if isset($ErrorPage.code.obsoleteData)}
      <p class="giDescription">
	{g->text text="Your change cannot be completed because somebody else has made a conflicting change to the same item.  Use the back button in your browser to go back to the page you were on, then <b>reload that page</b> and try your change again."}
        <br/>
        <a href="javascript:history.back()"> {g->text text="Go back and try again"} </a>
      </p>
      {if $ErrorPage.isAdmin}
      <p class="giDescription">
	{g->text text="If this problem happens repeatedly, it may be because of corruption in your cache.  Site Administrators can clear out this cache."}
        <br/>
        <a href="{g->url href="lib/support?cache"}"> {g->text text="Clear the cache"} </a>
      </p>
      {/if}
      <p class="giDescription" style="margin-top: 0.5em">
	{g->text text="Alternatively, you can return to the main Gallery page and resume browsing."}
      </p>
    {elseif isset($ErrorPage.code.securityViolation)}
      <p class="giDescription">
	{g->text text="The action you attempted is not permitted."}
      </p>
    {elseif isset($ErrorPage.code.requestAuthenticationFailure)}
      <p class="giDescription">
	{g->text text="Your change cannot be completed due to a loss of session data. Please try again. If it still doesn't work, try logging out and logging back in."}
      </p>
    {elseif isset($ErrorPage.code.storageFailure)}
      <p class="giDescription">
	{g->text text="An error has occurred while interacting with the database."}
      </p>
      {if $ErrorPage.isAdmin && !isset($ErrorPage.debug)}
	{g->text text="The exact nature of database errors is not captured unless Gallery debug mode is enabled in config.php.  Before seeking support for this error please enable buffered debug output and retry the operation.  Look near the bottom of the lengthy debug output to find error details."}
      {/if}
    {elseif isset($ErrorPage.code.platformFailure)}
      <p class="giDescription">
	{g->text text="An error has occurred while interacting with the platform."}
      </p>
      {if $ErrorPage.isAdmin && !isset($ErrorPage.debug)}
	{g->text text="The exact nature of the platform error is unknown. A common cause are insufficient file system permissions. This can happen if you or your webhost changed something in the file system, e.g. by restoring data from a backup."}
      {/if}
    {elseif isset($ErrorPage.code.missingObject)}
      <p class="giDescription">
	{g->text text="Item not found."}
      </p>
    {else}
      <p class="giDescription">
	{g->text text="An error has occurred."}
      </p>
    {/if}

    <p class="giDescription">
      <a href="{g->url}"> {g->text text="Back to the Gallery"} </a>
    </p>
  </div>

  {if !empty($ErrorPage.stackTrace)}
    <div class="gbBlock">
      <h3>
	{g->text text="Error Detail"}
	<span id="trace-toggle" class="giBlockToggle gcBackground1 gcBorder2"
	 style="border-width: 1px" onclick="BlockToggle('giStackTrace', 'trace-toggle')"> {if $ErrorPage.isAdmin}-{else}+{/if} </span>
      </h3>
      <div id="giStackTrace" style="margin-left: 0.8em{if !$ErrorPage.isAdmin}; display: none{/if}">
	{$ErrorPage.stackTrace}
      </div>
    </div>
  {/if}

  {if $ErrorPage.isAdmin}
    <div class="gbBlock">
      <h3> {g->text text="System Information"} </h3>
      <table class="gbDataTable"><tr>
	<td>
	  {g->text text="Gallery version"}
	</td><td>
	  {$ErrorPage.version}
	</td>
      </tr><tr>
	<td>
	  {g->text text="PHP version"}
	</td><td>
	  {$ErrorPage.phpversion} {$ErrorPage.php_sapi_name}
	</td>
       </tr><tr>
	<td>
	  {g->text text="Webserver"}
	</td><td>
	  {$ErrorPage.webserver}
	</td>
      </tr>
      {if isset($ErrorPage.dbType)}
      <tr>
	<td>
	  {g->text text="Database"}
	</td><td>
	  {$ErrorPage.dbType} {$ErrorPage.dbVersion}
	</td>
      </tr>
      {/if}
      {if isset($ErrorPage.toolkits)}
      <tr>
	<td>
	  {g->text text="Toolkits"}
	</td><td>
	  {$ErrorPage.toolkits}
	</td>
      </tr>
      {/if}
      <tr>
	<td>
	  {g->text text="Operating system"}
	</td><td>
	  {$ErrorPage.php_uname}
	</td>
      </tr><tr>
	<td>
	  {g->text text="Browser"}
	</td><td>
	  {$ErrorPage.browser}
	</td>
      </tr></table>
    </div>
  {/if}

  {if isset($ErrorPage.debug)}
    <div class="gbBlock">
      {include file="gallery:templates/debug.tpl"}
    </div>
  {/if}

  {if isset($ErrorPage.profile)}
    <div class="gbBlock">
      {include file="gallery:templates/profile.tpl"}
    </div>
  {/if}
</div>
