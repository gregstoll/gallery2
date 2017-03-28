{*
 * $Revision: 17662 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock">
  {if $ItemEditItem.can.changePathComponent}
  <div>
    <h2>
      {g->text text="Name"}
      <span class="giSubtitle">
	{g->text text="(required)"}
      </span>
    </h2>

    <p class="giDescription">
      {g->text text="The name of this item on your hard disk.  It must be unique in this album.  Only use alphanumeric characters, underscores or dashes."}
    </p>

    {strip}
    {foreach from=$ItemAdmin.parents item=parent}
    {if empty($parent.parentId)}
    /
    {else}
    {$parent.pathComponent}/
    {/if}
    {/foreach}
    {/strip}
    <input type="text" size="40"
     name="{g->formVar var="form[pathComponent]"}" value="{$form.pathComponent}"/>

    {if isset($form.error.pathComponent.invalid)}
    <div class="giError">
      {g->text text="Your name contains invalid characters.  Please choose another."}
    </div>
    {/if}
    {if isset($form.error.pathComponent.missing)}
    <div class="giError">
      {g->text text="You must enter a name for this item."}
    </div>
    {/if}
    {if isset($form.error.pathComponent.collision)}
    <div class="giError">
      {g->text text="The name you entered is already in use.  Please choose another."}
    </div>
    {/if}
  </div>
  {/if}

  <div>
    <h4> {g->text text="Title"} </h4>

    <p class="giDescription">
      {g->text text="The title of this item."}
    </p>

    {include file="gallery:modules/core/templates/MarkupBar.tpl"
	     viewL10domain="modules_core"
	     element="title" firstMarkupBar=true}

    <input type="text" id="title" size="60" maxlength="{$ItemEdit.fieldLengths.title}"
     name="{g->formVar var="form[title]"}" value="{$form.title}"/>

    {if !empty($form.error.title.missingRootTitle)}
    <div class="giError">
      {g->text text="The root album must have a title."}
    </div>
    {/if}
  </div>

  <div>
    <h4> {g->text text="Summary"} </h4>

    <p class="giDescription">
      {g->text text="The summary of this item."}
    </p>

    {include file="gallery:modules/core/templates/MarkupBar.tpl"
	     viewL10domain="modules_core"
	     element="summary"}
    <input type="text" id="summary" size="60"
     name="{g->formVar var="form[summary]"}" value="{$form.summary}"/>
  </div>

  <div>
    <h4> {g->text text="Keywords"} </h4>

    <p class="giDescription">
      {g->text text="Keywords are not visible, but are searchable."}
    </p>

    <textarea rows="2" cols="60"
     name="{g->formVar var="form[keywords]"}">{$form.keywords}</textarea>
  </div>

  <div>
    <h4> {g->text text="Description"} </h4>

    <p class="giDescription">
      {g->text text="This is the long description of the item."}
    </p>

    {include file="gallery:modules/core/templates/MarkupBar.tpl"
	     viewL10domain="modules_core"
	     element="description"}
    <textarea id="description" rows="4" cols="60"
     name="{g->formVar var="form[description]"}">{$form.description}</textarea>
  </div>
</div>

<div class="gbBlock">
  <h3> {g->text text="%s Date and Time" arg1=$ItemEditItem.typeName.0
	postSprintfArg1=$ItemEditItem.typeName.2} </h3>
	{* Specific translations: {g->text text="Link Date and Time"} *}

  <p class="giDescription">
    {if !empty($ItemEditItem.isItemPhoto)}
      {g->text text="Set the date and time when this image was captured."}
    {elseif !empty($ItemEditItem.isItemUnknown)}
      {g->text text="Set the date and time to be displayed for this item."}
    {else}
      {g->text text="Set the date and time to be displayed for this %s."
	       arg1=$ItemEditItem.typeName.1 postSprintfArg1=$ItemEditItem.typeName.3}
      {* Specific translations:
	 {g->text text="Set the date and time to be displayed for this link."} *}
    {/if}
  </p>

  <p>
    {capture name=originationTimestampField}{strip}
      {g->formVar var="form[originationTimestampSplit]"}
    {/strip}{/capture}
    {g->text text="Date:"}
    {capture name=htmlSelectDate}
      {html_select_date time=$form.originationTimestamp
       field_array=$smarty.capture.originationTimestampField start_year="1970" end_year="+0"}
    {/capture}
    {$smarty.capture.htmlSelectDate|utf8}
    {g->text text="Time:"}
    {html_select_time time=$form.originationTimestamp
     field_array=$smarty.capture.originationTimestampField}
    <br/>
  </p>

  {if !empty($ItemEditItem.originationTimestamp)}
  <script type="text/javascript">
  // <![CDATA[
  function setOriginationTimestamp() {ldelim}
    var frm = document.getElementById('itemAdminForm');
    frm.elements['{$smarty.capture.originationTimestampField}[Date_Month]'].value = '{$ItemEditItem.originationTimestamp.Date_Month}';
    frm.elements['{$smarty.capture.originationTimestampField}[Date_Day]'].value = '{$ItemEditItem.originationTimestamp.Date_Day}';
    frm.elements['{$smarty.capture.originationTimestampField}[Date_Year]'].value = '{$ItemEditItem.originationTimestamp.Date_Year}';
    frm.elements['{$smarty.capture.originationTimestampField}[Time_Hour]'].value = '{$ItemEditItem.originationTimestamp.Time_Hour}';
    frm.elements['{$smarty.capture.originationTimestampField}[Time_Minute]'].value = '{$ItemEditItem.originationTimestamp.Time_Minute}';
    frm.elements['{$smarty.capture.originationTimestampField}[Time_Second]'].value = '{$ItemEditItem.originationTimestamp.Time_Second}';
  {rdelim}
  // ]]>
  </script>
  <p>
    {g->text text="Use the original capture date and time from file information (e.g. Exif tag):"}
    <br/>
    <a href="#" onclick="setOriginationTimestamp();return false">
      {g->date timestamp=$ItemEditItem.originationTimestamp.timestamp style="datetime"}
    </a>
  </p>
  {/if}

  {if !empty($form.error.originationTimestamp.invalid)}
  <div class="giError">
    {g->text text="You must enter a valid date and time"}
  </div>
  {/if}
</div>

{if $ItemEditItem.can.editThumbnail}
<div class="gbBlock">
  <h3> {g->text text="Thumbnail"} </h3>

  <p class="giDescription">
    {g->text text="Set the size of the thumbnail.  The largest side of the thumbnail will be no larger than this value. Leave this field blank if you don't want a thumbnail."}
  </p>

  {if $ItemEditItem.can.createThumbnail}
    <input type="text" size="6"
     name="{g->formVar var="form[thumbnail][size]"}" value="{$form.thumbnail.size}"/>
  {else}
    <b>
    {g->text text="There are no graphics toolkits enabled that support this type of item, so we cannot create or modify a thumbnail."}
    {if $user.isAdmin}
      <a href="{g->url arg1="view=core.SiteAdmin" arg2="subView=core.AdminPlugins"}">
	{g->text text="site admin"}
      </a>
    {/if}
    </b>
  {/if}

  {if !empty($form.error.thumbnail.size.invalid)}
  <div class="giError">
    {g->text text="You must enter a number (greater than zero)"}
  </div>
  {/if}
  {if !empty($form.error.thumbnail.create)}
  <div class="giError">
    {g->text text="Unable to create a thumbnail for this item"}
  </div>
  {/if}
</div>
{/if}

{* Include our extra ItemEditOptions *}
{foreach from=$ItemEdit.options item=option}
  {include file="gallery:`$option.file`" l10Domain=$option.l10Domain}
{/foreach}

<div class="gbBlock gcBackground1">
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][save]"}" value="{g->text text="Save"}"/>
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][undo]"}" value="{g->text text="Reset"}"/>
</div>
