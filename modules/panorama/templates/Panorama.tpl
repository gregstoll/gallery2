{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gsContent">
  <div class="gbBlock gcBackground1">
    <h2> {g->text text=$Panorama.item.title|default:$Panorama.item.pathComponent} </h2>
  </div>

  <div class="gbBlock">
    {$Panorama.appletHtml}
  </div>
</div>
