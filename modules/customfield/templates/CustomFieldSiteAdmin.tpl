{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="Custom Fields"} </h2>
</div>

<div class="gbBlock">
  <p class="giDescription">
    {g->text text="These are the global settings for custom fields. They can be overridden at the album level. Common fields are available on all Gallery items; Album and Photo fields can be assigned only to items of the appropriate type."}
  </p>
</div>

<script type="text/javascript">
  // <![CDATA[
  var removeWarning = '{g->text text="WARNING: All values for this custom field will be deleted! (Except in albums with album-specific settings)" forJavascript=true}';
  var albumWarning = '{g->text text="WARNING: Values for this custom field on non-album items will be deleted! (Except in albums with album-specific settings)" forJavascript=true}';
  var photoWarning = '{g->text text="WARNING: Values for this custom field on non-photo items will be deleted! (Except in albums with album-specific settings)" forJavascript=true}';
  // ]]>
</script>

{include file="gallery:modules/customfield/templates/Admin.tpl"}
