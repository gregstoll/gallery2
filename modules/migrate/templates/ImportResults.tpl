{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="Import Complete"} </h2>
</div>

<div class="gbBlock">
  <h2 class="giSuccess">
  {if ($ImportResults.counts.userImportSuccess)}
    {g->text one="Successfully imported %d user." many="Successfully imported %d users."
	     count=$ImportResults.counts.userImportSuccess
             arg1=$ImportResults.counts.userImportSuccess}
  {else}
    {g->text text="No users imported."}
  {/if}
  </h2>

  {if count($ImportResults.status.userImportSuccess) > 0}
    <ul>
      {foreach from=$status.userImportSuccess key=username item=junk}
      <li>
	{g->text text="Imported %s" arg1=$username}
      </li>
      {/foreach}
    </ul>
  {/if}
</div>

{if count($ImportResults.status.userImportFailure) > 0}
<div class="gbBlock">
  <h2 class="giError">
  {if ($ImportResults.counts.userImportFailure)}
    {g->text one="Error while importing %d user." many="Errors while importing %d users."
	     count=$ImportResults.counts.userImportFailure
	     arg1=$ImportResults.counts.userImportFailure}
  {/if}
  </h2>

  <ul>
  {foreach from=$ImportResults.status.userImportFailure key=username item=junk}
    <li>
      {g->text text="Error importing %s" arg1=$username}
    </li>
  {/foreach}
  </ul>
</div>
{/if}

<div class="gbBlock">
  <h2 class="giSuccess">
    {if ($ImportResults.counts.albumImportSuccess)}
      {g->text one="Successfully imported %d album." many="Successfully imported %d albums."
	       count=$ImportResults.counts.albumImportSuccess
	       arg1=$ImportResults.counts.albumImportSuccess}
    {else}
      {g->text text="No albums imported."}
    {/if}
  </h2>

  {if sizeof($ImportResults.status.albumImportSuccess) > 0}
    <ul>
      {foreach from=$status.albumImportSuccess key=albumname item=junk}
      <li>
	{g->text text="Imported %s" arg1=$albumname|htmlentities}
      </li>
      {/foreach}
    </ul>
  {/if}
</div>

{if sizeof($ImportResults.status.albumImportFailure) > 0}
<div class="gbBlock">
  <h2 class="giError">
    {if ($ImportResults.counts.albumImportFailure)}
      {g->text one="Error while importing %d album." many="Errors while importing %d albums."
	       count=$ImportResults.counts.albumImportFailure
	       arg1=$ImportResults.counts.albumImportFailure}
    {/if}
  </h2>

  <ul>
    {foreach from=$ImportResults.status.albumImportFailure key=albumname item=junk}
    <li>
      {g->text text="Error importing %s" arg1=$albumname|htmlentities}
    </li>
    {/foreach}
  </ul>
</div>
{/if}

{if $ImportResults.status.urlRedirect}
<div class="gbBlock">
  <h3> {g->text text="URL Redirection"} </h3>
  {include file="gallery:modules/migrate/templates/Redirect.tpl"}
</div>
{/if}

<div class="gbBlock">
  <a href="{g->url arg1="view=core.SiteAdmin" arg2="subView=migrate.SelectGallery"}">
    {g->text text="Import more data"}
  </a>
</div>
