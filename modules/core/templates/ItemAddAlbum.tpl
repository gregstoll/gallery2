{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="Add Sub-Album"} </h2>
</div>

<div class="gbBlock">
  <h4>
    {g->text text="Name"}
    <span class="giSubtitle"> {g->text text="(required)"} </span>
  </h4>

  <p class="giDescription">
    {g->text text="The name of this album on your hard disk.  It must be unique in this album.  Only use alphanumeric characters, underscores or dashes.  You will be able to rename it later."}
  </p>

  {strip}
  {foreach from=$ItemAdmin.parents item=parent}
    {$parent.pathComponent}/
  {/foreach}
  {$ItemAdmin.item.pathComponent}/
  {/strip}

  <input type="text" size="10"
   name="{g->formVar var="form[pathComponent]"}" value="{$form.pathComponent}"/>

  <script type="text/javascript">
    document.getElementById('itemAdminForm')['{g->formVar var="form[pathComponent]"}'].focus();
  </script>

  {if !empty($form.error.pathComponent.invalid)}
  <div class="giError">
    {g->text text="Your name contains invalid characters.  Please enter another."}
  </div>
  {/if}
  {if !empty($form.error.pathComponent.missing)}
  <div class="giError">
    {g->text text="You must enter a name for this album."}
  </div>
  {/if}
  {if !empty($form.error.pathComponent.collision)}
  <div class="giError">
    {g->text text="The name you entered is already in use.  Please enter another."}
  </div>
  {/if}

  <h4> {g->text text="Title"} </h4>
  <p class="giDescription">
    {g->text text="This is the album title."}
  </p>

  {include file="gallery:modules/core/templates/MarkupBar.tpl"
	   viewL10domain="modules_core"
	   element="title" firstMarkupBar=true}

  <input type="text" id="title" size="40"
   name="{g->formVar var="form[title]"}" value="{$form.title}"/>

  <h4> {g->text text="Summary"} </h4>
  <p class="giDescription">
    {g->text text="This is the album summary."}
  </p>

  {include file="gallery:modules/core/templates/MarkupBar.tpl"
	   viewL10domain="modules_core"
	   element="summary"}

  <input type="text" id="summary" size="40"
   name="{g->formVar var="form[summary]"}" value="{$form.summary}"/>

  <h4> {g->text text="Keywords"} </h4>
  <p class="giDescription">
    {g->text text="Keywords are not visible, but are searchable."}
  </p>

  <textarea rows="2" cols="60"
   name="{g->formVar var="form[keywords]"}">{$form.keywords}</textarea>

  <h4> {g->text text="Description"} </h4>
  <p class="giDescription">
    {g->text text="This is the long description of the album."}
  </p>

  {include file="gallery:modules/core/templates/MarkupBar.tpl"
	   viewL10domain="modules_core"
	   element="description"}

  <textarea id="description" rows="4" cols="60"
   name="{g->formVar var="form[description]"}">{$form.description}</textarea>
</div>

<div class="gbBlock gcBackground1">
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][create]"}" value="{g->text text="Create"}"/>
</div>
