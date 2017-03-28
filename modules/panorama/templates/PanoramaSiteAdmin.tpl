{*
 * $Revision: 17380 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="Panorama Settings"} </h2>
</div>

{if !empty($status)}
<div class="gbBlock"><h2 class="giSuccess">
  {if isset($status.saved)}
    {g->text text="Settings saved successfully"}
  {/if}
  {if isset($status.reset)}
    {g->text text="Items reset successfully"}
  {/if}
  {if isset($status.deactivated)}
  <span class="giError">
    {g->text text="Reset panorama items to enable deactivation (see below)"}
  </span>
  {/if}
</h2></div>
{/if}

<div class="gbBlock">
  <p class="giDescription">
    {g->text text="The panorama viewer can be activated in two ways: the first allows album administrators to select individual images for panorama display (Panorama section in \"edit photo\"), overriding the normal display of the entire image.  The second method retains the normal image display but gives users an option in the \"item actions\" to view the image in the panorama viewer."}
  </p>
  <p style="line-height: 2.5em; margin-left: 1em">
    <input type="checkbox" id="cbItemType" {if $form.itemType}checked="checked" {/if}
     name="{g->formVar var="form[itemType]"}"/>
    <label for="cbItemType">
      {g->text text="Use applet to display wide images"}
    </label>
    <br/>

    <input type="checkbox" id="cbItemLink" {if $form.itemLink}checked="checked" {/if}
     name="{g->formVar var="form[itemLink]"}"/>
    <label for="cbItemLink">
      {g->text text="Add \"view panorama\" option in item actions for wide images"}
    </label>
    <br/>

    {g->text text="Width of panorama viewer: "}
    <input type="text" size="6" name="{g->formVar var="form[width]"}" value="{$form.width}"/>

    {if isset($form.error.width)}
    <div class="giError">
      {g->text text="Invalid width value"}
    </div>
    {/if}
  </p>
</div>

<div class="gbBlock gcBackground1">
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][save]"}" value="{g->text text="Save"}"/>
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][undo]"}" value="{g->text text="Reset"}"/>
</div>
