{*
 * $Revision: 17380 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="getID3() Settings"} </h2>
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
    {g->text text="getID3() is a PHP4 script that extracts useful information (such as ID3 tags, bitrate, playtime, etc.) from MP3s &amp; other multimedia file formats (Ogg, WMA, WMV, ASF, WAV, AVI, AAC, VQF, FLAC, MusePack, Real, QuickTime, Monkey's Audio, MIDI and more)."}
  </p>
</div>

<div class="gbBlock">
  <h3> {g->text text="Summary and Detailed displays"} </h3>

  <p class="giDescription">
    {g->text text="There can be a great deal of information stored in many file formats.  We display that data in two different views, summary and detailed.  You can choose which properties are displayed in each view."}
  </p>

  <table class="gbDataTable"><tr>
    <th> {g->text text="Available"} </th>
    <th> {g->text text="Summary"} </th>
    <th> {g->text text="Detailed"} </th>
  </tr><tr>
    <td>
      <select name="{g->formVar var="form[available][]"}" size="20" multiple="multiple">
        {html_options options=$AdminGetid3.availableList}
      </select>
    </td><td>
      <select name="{g->formVar var="form[summary][]"}" size="20" multiple="multiple">
        {html_options options=$AdminGetid3.summaryList}
      </select>
    </td><td>
       <select name="{g->formVar var="form[detailed][]"}" size="20" multiple="multiple">
         {html_options options=$AdminGetid3.detailedList}
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
    {g->text text="When MP3s are added to Gallery check the ID3 title field and apply to:"}
  </p>
  <p class="giDescription">
    <input type="checkbox" id="cbMp3Title" {if $form.item.mp3title}checked="checked" {/if}
     name="{g->formVar var="form[item][mp3title]"}"/>
    <label for="cbMp3Title">
      {g->text text="Title"}
    </label>
  </p>
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][save]"}" value="{g->text text="Save"}"/>
</div>

