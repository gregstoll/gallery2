{*
 * $Revision: 17380 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="EXIF/IPTC Settings"} </h2>
</div>

{if !empty($status)}
<div class="gbBlock"><h2 class="giSuccess">
  {if isset($status.added.summary)}
    {g->text one="Added %d property to the Summary view"
	     many="Added %d properties to the Summary view"
	     count=$status.added.summary arg1=$status.added.summary}
  {/if}
  {if isset($status.removed.summary)}
    {g->text one="Removed %d property from the Summary view"
	     many="Removed %d properties from the Summary view"
	     count=$status.removed.summary arg1=$status.removed.summary}
  {/if}
  {if isset($status.restored.summary)}
    {g->text text="Restored the default properties for the Summary view"}
  {/if}
  {if isset($status.movedUp.summary)}
    {g->text one="Moved %d property up in the Summary view"
	     many="Moved %d properties up in the Summary view"
	     count=$status.movedUp.summary arg1=$status.movedUp.summary}
  {/if}
  {if isset($status.movedDown.summary)}
    {g->text one="Moved %d property down in the Summary view"
	     many="Moved %d properties down in the Summary view"
	     count=$status.movedDown.summary arg1=$status.movedDown.summary}
  {/if}
  {if isset($status.added.detailed)}
    {g->text one="Added %d property to the Detailed view"
	     many="Added %d properties to the Detailed view"
	     count=$status.added.detailed arg1=$status.added.detailed}
  {/if}
  {if isset($status.removed.detailed)}
    {g->text one="Removed %d property from the Detailed view"
	     many="Removed %d properties from the Detailed view"
	     count=$status.removed.detailed arg1=$status.removed.detailed}
  {/if}
  {if isset($status.restored.detailed)}
    {g->text text="Restored the default properties for the Detailed view"}
  {/if}
  {if isset($status.movedUp.detailed)}
    {g->text one="Moved %d property up in the Detailed view"
	     many="Moved %d properties up in the Detailed view"
	     count=$status.movedUp.detailed arg1=$status.movedUp.detailed}
  {/if}
  {if isset($status.movedDown.detailed)}
    {g->text one="Moved %d property down in the Detailed view"
	     many="Moved %d properties down in the Detailed view"
	     count=$status.movedDown.detailed arg1=$status.movedDown.detailed}
  {/if}
  {if isset($status.saved)}
    {g->text text="Settings saved successfully"}
  {/if}
</h2></div>
{/if}

<div class="gbBlock">
  <p class="giDescription">
    {g->text text="Gallery can display the EXIF data that is embedded in photos taken by most digital cameras. Gallery can also display IPTC data that was added to the photos by some IPTC enabled software."}
  </p>
</div>

<div class="gbBlock">
  <h3> {g->text text="Summary and Detailed EXIF/IPTC displays"} </h3>

  <p class="giDescription">
    {g->text text="There can be a great deal of EXIF/IPTC information stored in photos.  We display that data in two different views, summary and detailed.  You can choose which properties are displayed in each view."}
  </p>

  <table class="gbDataTable"><tr>
    <th> {g->text text="Available"} </th>
    <th> {g->text text="Summary"} </th>
    <th> {g->text text="Detailed"} </th>
  </tr><tr>
    <td>
      <select name="{g->formVar var="form[available][]"}" size="20" multiple="multiple">
	{html_options options=$AdminExif.availableList}
      </select>
    </td><td>
      <select name="{g->formVar var="form[summary][]"}" size="20" multiple="multiple">
	{html_options options=$AdminExif.summaryList}
      </select>
    </td><td>
      <select name="{g->formVar var="form[detailed][]"}" size="20" multiple="multiple">
	{html_options options=$AdminExif.detailedList}
      </select>
    </td>
  </tr>

  {if isset($form.error.available.missing) ||
      isset($form.error.summary.missing) || isset($form.error.detailed.missing)}
  <tr>
    <td colspan="3">
      <div class="giError">
	{if isset($form.error.available.missing)}
	  {g->text text="You must select at least one value in the available column"}
	{/if}
	{if isset($form.error.summary.missing)}
	  {g->text text="You must select at least one value in the summary column"}
	{/if}
	{if isset($form.error.detailed.missing)}
	  {g->text text="You must select at least one value in the detailed column"}
	{/if}
      </div>
    </td>
  </tr>
  {/if}

  <tr>
    <td>
      <input type="submit" class="inputTypeSubmit"
       name="{g->formVar var="form[action][availableToSummary]"}"
       value="{g->text text="Add to Summary"}"/>
      <input type="submit" class="inputTypeSubmit"
       name="{g->formVar var="form[action][availableToDetailed]"}"
       value="{g->text text="Add to Detailed"}"/>
    </td><td>
      <input type="submit" class="inputTypeSubmit"
       name="{g->formVar var="form[action][removeFromSummary]"}" value="{g->text text="Remove"}"/>
      <input type="submit" class="inputTypeSubmit"
       name="{g->formVar var="form[action][moveUpSummary]"}" value="{g->text text="Up"}"/>
      <input type="submit" class="inputTypeSubmit"
       name="{g->formVar var="form[action][moveDownSummary]"}" value="{g->text text="Down"}"/>
    </td><td>
      <input type="submit" class="inputTypeSubmit"
       name="{g->formVar var="form[action][removeFromDetailed]"}" value="{g->text text="Remove"}"/>
      <input type="submit" class="inputTypeSubmit"
       name="{g->formVar var="form[action][moveUpDetailed]"}" value="{g->text text="Up"}"/>
      <input type="submit" class="inputTypeSubmit"
       name="{g->formVar var="form[action][moveDownDetailed]"}" value="{g->text text="Down"}"/>
    </td>
  </tr></table>
</div>

<div class="gbBlock">
  <h3> {g->text text="Reset to Defaults"} </h3>

  <p class="giDescription">
    {g->text text="Restore the original values for the Summary and Detailed views.  Use with caution, there is no undo!"}
  </p>

  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][resetSummary]"}"
   value="{g->text text="Restore Summary Defaults"}"/>
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][resetDetailed]"}"
   value="{g->text text="Restore Detailed Defaults"}"/>
</div>

<div class="gbBlock">
  <h3> {g->text text="Item Upload"} </h3>

  <p class="giDescription">
    {g->text text="When photos are added to Gallery check for EXIF Image Description and apply to:"}
  </p>
  <p class="giDescription">
    <input type="checkbox" id="cbItemSummary" {if $form.item.summary}checked="checked" {/if}
     name="{g->formVar var="form[item][summary]"}"/>
    <label for="cbItemSummary">
      {g->text text="Item Summary"}
    </label>
    <br/>

    <input type="checkbox" id="cbItemDescription" {if $form.item.description}checked="checked" {/if}
     name="{g->formVar var="form[item][description]"}"/>
    <label for="cbItemDescription">
      {g->text text="Item Description"}
    </label>
  </p>
  <p class="giDescription">
    {g->text text="When photos are added to Gallery check for IPTC Keywords and apply to:"}
  </p>
  <p class="giDescription">
    <input type="checkbox" id="cbItemKeywords" {if $form.item.keywords}checked="checked" {/if}
     name="{g->formVar var="form[item][keywords]"}"/>
    <label for="cbItemKeywords">
      {g->text text="Item Keywords"}
    </label>
  </p>
  <p class="giDescription">
    {g->text text="When photos are added to Gallery check for IPTC Object Name and apply to:"}
  </p>
  <p class="giDescription">
    <input type="checkbox" id="cbItemObjectName" {if $form.item.objectName}checked="checked" {/if}
     name="{g->formVar var="form[item][objectName]"}"/>
    <label for="cbItemObjectName">
      {g->text text="Item Title"}
    </label>
  </p>
  <p class="giDescription">
    {g->text text="When photos are added should we rotate them based on EXIF orientation data?"}
  </p>
  <p class="giDescription">
    <input type="checkbox" id="cbItemExifRotate" {if $form.item.exifrotate}checked="checked" {/if}
     name="{g->formVar var="form[item][exifrotate]"}"/>
    <label for="cbItemExifRotate">
      {g->text text="Rotate pictures automatically"}
    </label>
  </p>
  {if !$AdminExif.canRotate}
  <p class="giWarning">
    {g->text text="Warning: No toolkit support for rotation of JPEG images.  Make sure to activate a toolkit module if using this option."}
  </p>
  {/if}
  <p class="giDescription">
    <input type="checkbox" id="cbItemExifRotatePreserve" {if $form.item.exifrotatepreserve}checked="checked" {/if}
     name="{g->formVar var="form[item][exifrotatepreserve]"}"/>
    <label for="cbItemExifRotatePreserve">
      {g->text text="Preserve Original on Rotating"}
    </label>
  </p>
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][save]"}" value="{g->text text="Save"}"/>
</div>
