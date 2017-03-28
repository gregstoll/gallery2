{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<form action="{g->url}" enctype="application/x-www-form-urlencoded" method="post"
 id="publishXpForm">
  <div>      
    <script type="text/javascript">
      // <![CDATA[
      setSubtitle("{g->text text="Specify the name and title for the new album." forJavascript=true}");
      setSubmitOnNext(true);
      setOnBackUrl("{g->url arg1="view=publishxp.SelectAlbum" htmlEntities=false}");
      setButtons(true, true, false);
      // ]]>
    </script>
    {g->hiddenFormVars}
    <input type="hidden" name="{g->formVar var="controller"}" value="{$controller}"/>
    <input type="hidden" name="{g->formVar var="form[formName]"}" value="{$form.formName}"/>
    <input type="hidden" name="{g->formVar var="form[parentId]"}" value="{$form.parentId}"/>
    <input type="hidden" name="{g->formVar var="form[action][newAlbum]"}" value="1"/>
  </div>
 
  <div class="gbBlock gcBackground1">
    <h2>
      {g->text text="Create a new album"}
    </h2>
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
    {foreach from=$NewAlbum.parents item=parent}
    {$parent.pathComponent}/
    {/foreach}
    {/strip}

    <input type="text" size="10"
     name="{g->formVar var="form[pathComponent]"}" value="{$form.pathComponent}"/>
    <script type="text/javascript">document.getElementById('publishXpForm')['{g->formVar
     var="form[pathComponent]"}'].focus();</script>

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
      
    <h4>
      {g->text text="Title"}
    </h4>
      
    <p class="giDescription">
      {g->text text="This is the album title."}
    </p>

    {include file="gallery:modules/core/templates/MarkupBar.tpl" 
             viewL10domain="modules_core" 
             element="title"
	     firstMarkupBar=true}
    <input type="text" id="title" size="40" name="{g->formVar var="form[title]"}" value="{$form.title}"/>
    {if !empty($form.error.title.missing)}
    <div class="giError">
      {g->text text="You must enter a name for this album."}
    </div>
    {/if}

    <h4>
      {g->text text="Summary"}
    </h4>

    <p class="giDescription">
      {g->text text="This is the album summary."}
    </p>

    {include file="gallery:modules/core/templates/MarkupBar.tpl" 
             viewL10domain="modules_core" 
             element="summary"}
    <input type="text" id="summary" size="40" name="{g->formVar var="form[summary]"}" value="{$form.summary}"/>

    <h4>
      {g->text text="Keywords"}
    </h4>

    <p class="giDescription">
      {g->text text="Keywords are not visible, but are searchable."}
    </p>

    <textarea rows="2" cols="58" name="{g->formVar var="form[keywords]"}">{$form.keywords}</textarea>

    <h4 class="giTitle">
      {g->text text="Description"}
    </h4>

    <p class="giDescription">
      {g->text text="This is the long description of the album."}
    </p>

    {include file="gallery:modules/core/templates/MarkupBar.tpl" 
             viewL10domain="modules_core" 
	     element="description"}
    <textarea id="description" rows="4" cols="58" name="{g->formVar var="form[description]"}">{$form.description}</textarea>
  </div>
</form>
