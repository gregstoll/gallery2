{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="Import Complete"} </h2>
</div>


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
        <li> {g->text text="Imported %s" arg1=$albumname} </li>
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
          <li> {g->text text="Error importing %s" arg1=$albumname} </li>
        {/foreach}
      </ul>
  </div>
{/if}

<div class="gbBlock">
  <h2 class="giSuccess">
    {if ($ImportResults.counts.pictureImportSuccess)}
      {g->text one="Successfully imported %d picture." many="Successfully imported %d pictures."
        count=$ImportResults.counts.pictureImportSuccess
        arg1=$ImportResults.counts.pictureImportSuccess}
    {else}
      {g->text text="No pictures imported."}
    {/if}
  </h2>

  {if sizeof($ImportResults.status.pictureImportSuccess) > 0}
    <ul>
      {foreach from=$status.pictureImportSuccess key=picturename item=junk}
        <li> {g->text text="Imported %s" arg1=$picturename} </li>
      {/foreach}
    </ul>
  {/if}
</div>

{if sizeof($ImportResults.status.pictureImportFailure) > 0}
  <div class="gbBlock">
    <h2 class="giError">
      {if ($ImportResults.counts.pictureImportFailure)}
        {g->text one="Error while importing %d picture." many="Errors while importing %d pictures."
          count=$ImportResults.counts.pictureImportFailure
          arg1=$ImportResults.counts.pictureImportFailure}
      {/if}
    </h2>

    <ul>
      {foreach from=$ImportResults.status.pictureImportFailure key=picturename item=junk}
        <li> {g->text text="Error importing %s" arg1=$picturename} </li>
      {/foreach}
    </ul>
  </div>
{/if}

<div class="gbBlock">
  <a href="{g->url arg1="view=core.SiteAdmin" arg2="subView=picasa.SelectPicasaExportPath"}">
    {g->text text="Import another Picasa album"}
  </a>
  <br/>
  <a href="{g->url arg1="view=core.ShowItem" arg2="itemId=`$status.albumId`"}">
    {g->text text="View the imported album"}
  </a>
</div>
