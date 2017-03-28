{*
 * $Revision: 16827 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="Edit comment"} </h2>
</div>

{if isset($form.action.preview)}
<div class="gbBlock">
  <h3> {g->text text="Comment Preview"} </h3>

  <h4> {$form.subject|markup} </h4>

  {$form.comment|markup}
</div>
{/if}

<form action="{g->url}" method="post" enctype="application/x-www-form-urlencoded"
 id="editCommentForm">
  <div>
    {g->hiddenFormVars}
    <input type="hidden" name="{g->formVar var="controller"}" value="comment.EditComment"/>
    <input type="hidden" name="{g->formVar var="form[formName]"}" value="{$form.formName}"/>
    <input type="hidden" name="{g->formVar var="itemId"}" value="{$EditComment.itemId}"/>
  </div>

  <div class="gbBlock">
    <h4>
      {g->text text="Commenter"}
      <span class="giSubtitle"> {g->text text="(required)"} </span>
    </h4>

    <input type="hidden" name="{g->formVar var="commentId"}" value="{$EditComment.comment.id}"/>
    <input type="text" name="{g->formVar var="form[commenterName]"}" value="{$form.commenterName}"/>

    {if $EditComment.isGuestComment}
    <h4>
      {g->text text="Name"}
    </h4>
    <input type="text" name="{g->formVar var="form[author]"}" value="{$form.author}"/>
    {/if}

    {if isset($form.error.commenterName.missing)}
    <div class="giError">
      {g->text text="You must enter a username."}
    </div>
    {/if}
    {if isset($form.error.commenterName.invalid)}
    <div class="giError">
      {g->text text="The username you entered is invalid."}
    </div>
    {/if}

    <h4> {g->text text="Host"} </h4>
    <input type="text" name="{g->formVar var="form[host]"}" value="{$form.host}"/>

    <h4> {g->text text="Subject"} </h4>

    {include file="gallery:modules/core/templates/MarkupBar.tpl" viewL10domain="modules_core"
  	     element="subject" firstMarkupBar=true}

    <input type="text" id="subject" size="60" class="gcBackground1"
	   name="{g->formVar var="form[subject]"}" value="{$form.subject}"
	   onfocus="this.className=''" onblur="this.className='gcBackground1'"/>

    <script type="text/javascript">
      document.getElementById('editCommentForm')['{g->formVar var="form[subject]"}'].focus();
    </script>

    <h4>
      {g->text text="Comment"}
      <span class="giSubtitle"> {g->text text="(required)"} </span>
    </h4>

    {include file="gallery:modules/core/templates/MarkupBar.tpl" viewL10domain="modules_core"
  	     element="comment"}

    <textarea rows="15" cols="60" id="comment" class="gcBackground1"
	      name="{g->formVar var="form[comment]"}"
	      onfocus="this.className=''"
	      onblur="this.className='gcBackground1'">{$form.comment}</textarea>

    {if isset($form.error.comment.missing)}
    <div class="giError">
      {g->text text="You must enter a comment"}
    </div>
    {/if}

    <h4>
      {g->text text="Publish Status"}
    </h4>
    <select name="{g->formVar var="form[publishStatus]"}">
      {html_options options=$EditComment.publishStatusList selected=$form.publishStatus}
    </select>
  </div>

  <div class="gbBlock gcBackground1">
    <input type="submit" class="inputTypeSubmit"
	   name="{g->formVar var="form[action][preview]"}" value="{g->text text="Preview"}"/>
    <input type="submit" class="inputTypeSubmit"
	   name="{g->formVar var="form[action][save]"}" value="{g->text text="Save"}"/>
    <input type="submit" class="inputTypeSubmit"
	   name="{g->formVar var="form[action][cancel]"}" value="{g->text text="Cancel"}"/>
  </div>
</form>
