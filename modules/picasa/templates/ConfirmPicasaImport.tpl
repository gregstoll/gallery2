{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="Confirm Import"} </h2>
</div>

<div class="gbBlock">
  <h3> {g->text text="Album to import:"} </h3>

  <table class="gbDataTable"><tr>
    <th> {g->text text="Album Name"} </th>
    <th> {g->text text="Album Caption"} </th>
    <th> {g->text text="Album Notes"} </th>
  </tr>
  {assign var=albumName value=$ConfirmPicasaImport.album.albumName}
  <tr class="{cycle values="gbEven,gbOdd"}">
    <td>
      {$ConfirmPicasaImport.album.albumName}
      <input type="hidden" name="{g->formVar var="form[sourceAlbums][$albumName]"}" value="1"/>
    </td><td>
      {$ConfirmPicasaImport.album.albumCaption}
    </td>
    <td>
      {if (isset($ConfirmPicasaImport.existingAlbums.$albumName))}
        {g->text text="An album already exists with this name.  This album will be renamed."}
        <br/>
      {/if}
      {if (isset($ConfirmPicasaImport.illegalAlbumNames.$albumName))}
        {g->text text="This album has an illegal name and will be renamed to <i>%s</i>"
          arg1=$ConfirmPicasaImport.illegalAlbumNames.$albumName}
        <br/>
      {/if}
    </td>
  </tr>
  </table>
</div>

<div class="gbBlock">
  <p class="giDescription">
    {g->text text="The album will be imported into this album:"}
  </p>
  <div class="giInfo">
    <span>
      {g->text text="Title: %s"
        arg1=$ConfirmPicasaImport.targetAlbum.title|default:$ConfirmPicasaImport.targetAlbum.pathComponent|markup}
    </span><br/>
    <span>
      {g->text text="Description: %s" arg1=$ConfirmPicasaImport.targetAlbum.description|default:""|markup}
    </span>
  </div>
  <input type="hidden"
    name="{g->formVar var="form[destinationAlbumId]"}" value="{$ConfirmPicasaImport.destinationAlbumId}"/>
</div>

<div class="gbBlock gcBackground1">
  <input type="hidden" 
    name="{g->formVar var="form[picasaXmlPath]"}" value="{$ConfirmPicasaImport.picasaXmlPath}"/>
  <input type="submit" class="inputTypeSubmit"
    name="{g->formVar var="form[action][import]"}" value="{g->text text="Import"}"/>
  <input type="submit" class="inputTypeSubmit"
    name="{g->formVar var="form[action][cancel]"}" value="{g->text text="Cancel"}"/>
</div>
