{*
 * $Revision: 17662 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2>
    {g->text text="Edit Captions"}
    {if ($ItemEditCaptions.numPages > 1) }
      {g->text text="(page %d of %d)" arg1=$ItemEditCaptions.page arg2=$ItemEditCaptions.numPages}
    {/if}
  </h2>
</div>

{if !empty($status)}
<div class="gbBlock">
  {if $status.errorCount > 0}
  <h2 class="giError">
    {if $status.successCount > 0}
      {g->text text="There were errors saving some items"}
    {else}
      {g->text text="There were errors saving all items"}
    {/if}
  </h2>
  {elseif $status.successCount > 0}
  <h2 class="giSuccess">
    {g->text text="Successfully saved all items"}
  </h2>
  {/if}
</div>
{/if}

{if empty($form.items)}
  <div class="gbBlock">
    <p class="giDescription">
      {g->text text="This album contains no items"}
    </p>
  </div>
{else}

<input type="hidden" name="{g->formVar var="page"}" value="{$ItemEditCaptions.page}"/>
<input type="hidden" name="{g->formVar var="form[formname]"}" value="EditCaption"/>
<input type="hidden" name="{g->formVar var="form[numPerPage]"}" value="{$form.numPerPage}"/>

{foreach name=itemLoop from=$form.items item=item}
<div class="gbBlock">
  <input type="hidden"
   name="{g->formVar var="form[items][`$item.id`][serialNumber]"}" value="{$item.serialNumber}"/>

  {if isset($item.thumbnail)}{strip}
  <div style="float: right">
    <a id="thumb_{$item.id}" href="{g->url arg1="view=core.ShowItem" arg2="itemId=`$item.id`"}">
      {g->image item=$item image=$item.thumbnail maxSize=150 class="giThumbnail"}
    </a>
  </div>
  {/strip}{/if}

  <h4> {g->text text="Title"} </h4>
  {include file="gallery:modules/core/templates/MarkupBar.tpl"
	   viewL10domain="modules_core"
	   element="title_`$item.id`" firstMarkupBar=$smarty.foreach.itemLoop.first}
  <input type="text" id="title_{$item.id}" size="60" maxlength="{$ItemEditCaptions.fieldLengths.title}"
   name="{g->formVar var="form[items][`$item.id`][title]"}" value="{$item.title}"/>

  <h4> {g->text text="Summary"} </h4>
  {include file="gallery:modules/core/templates/MarkupBar.tpl"
	   viewL10domain="modules_core"
	   element="summary_`$item.id`" firstMarkupBar=false}
  <input type="text" id="summary_{$item.id}" size="60"
   name="{g->formVar var="form[items][`$item.id`][summary]"}" value="{$item.summary}"/>

  <h4> {g->text text="Keywords"} </h4>
  <textarea id="keywords_{$item.id}" rows="2" cols="60"
   name="{g->formVar var="form[items][`$item.id`][keywords]"}">{$item.keywords}</textarea>

  <h4> {g->text text="Description"} </h4>
  {include file="gallery:modules/core/templates/MarkupBar.tpl"
	   viewL10domain="modules_core"
	   element="description_`$item.id`" firstMarkupBar=false}
  <textarea id="description_{$item.id}" rows="4" cols="60"
   name="{g->formVar var="form[items][`$item.id`][description]"}">{$item.description}</textarea>

  {if isset($status[$item.id].saved)}
  <div class="giSuccess">
    {g->text text="Saved successfully."}
  </div>
  {/if}
  {if isset($status[$item.id].obsolete)}
  <div class="giError">
    {g->text text="This item was modified by somebody else at the same time.  Your changes were lost."}
  </div>
  {/if}
  {* We will probably  never see this message because the view won't show us items for which we have no permissions *}
  {if isset($status[$item.id].permissionDenied)}
  <div class="giError">
    {g->text text="You do not have permissions to modify this item."}
  </div>
  {/if}
</div>
{/foreach}
<script type="text/javascript">
  //<![CDATA[
  {foreach from=$form.items item=item}
  {if isset($item.resize)}
  {* force and alt/longdesc parameter here so that we avoid issues with single quotes in the title/description *}
  new YAHOO.widget.Tooltip("gTooltip", {ldelim}
      context: "thumb_{$item.id}", text: '{g->image item=$item image=$item.resize class="giThumbnail" maxSize=640 alt="" longdesc="" }',
      showDelay: 250 {rdelim});
  {elseif isset($item.thumbnail)}
  new YAHOO.widget.Tooltip("gTooltip", {ldelim}
      context: "thumb_{$item.id}", text: '{g->image item=$item image=$item.thumbnail class="giThumbnail" alt="" longdesc=""}',
      showDelay: 250 {rdelim});
  {/if}
  {/foreach}
  //]]>
</script>


<div class="gbBlock gcBackground1">
  {if $ItemEditCaptions.canCancel}
    <input type="submit" class="inputTypeSubmit"
     name="{g->formVar var="form[action][save][done]"}" value="{g->text text="Save and Done"}"/>
  {else}
    <input type="submit" class="inputTypeSubmit"
     name="{g->formVar var="form[action][save][stay]"}" value="{g->text text="Save"}"/>
  {/if}

  {if ($ItemEditCaptions.page > 1)}
    <input type="submit" class="inputTypeSubmit"
     name="{g->formVar var="form[action][save][previous]"}"
     value="&laquo; {g->text text="Save and Edit Previous %s" arg1=$form.numPerPage}"/>
  {/if}

  {if ($ItemEditCaptions.page < $ItemEditCaptions.numPages)}
    <input type="submit" class="inputTypeSubmit"
     name="{g->formVar var="form[action][save][next]"}"
     value="{g->text text="Save and Edit Next %s" arg1=$form.numPerPage} &raquo;"/>
  {/if}

  {if $ItemEditCaptions.canCancel}
      <input type="submit" class="inputTypeSubmit"
     name="{g->formVar var="form[action][cancel]"}" value="{g->text text="Cancel"}"/>
  {/if}
</div>
{/if}
