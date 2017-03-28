{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="Delete this comment?"} </h2>
</div>

<form action="{g->url}" method="post" enctype="application/x-www-form-urlencoded">
  <div>
    {g->hiddenFormVars}
    <input type="hidden" name="{g->formVar var="controller"}" value="comment.DeleteComment"/>
    <input type="hidden" name="{g->formVar var="form[formName]"}" value="{$form.formName}"/>
    <input type="hidden" name="{g->formVar var="itemId"}" value="{$DeleteComment.item.id}"/>
    <input type="hidden" name="{g->formVar var="commentId"}" value="{$DeleteComment.comment.id}"/>
  </div>

  <div class="gbBlock">
    <h3> {g->text text="Are you sure?"} </h3>
    <p class="giDescription">
      {g->text text="Delete this comment?  There is no undo!"}
    </p>
  </div>

  <div class="gbBlock gcBackground1">
    <input type="hidden" name="{g->formVar var="commentId"}" value="{$DeleteComment.comment.id}"/>
    <input type="submit" class="inputTypeSubmit"
           name="{g->formVar var="form[action][delete]"}" value="{g->text text="Delete"}"/>
    <input type="submit" class="inputTypeSubmit"
	   name="{g->formVar var="form[action][cancel]"}" value="{g->text text="Cancel"}"/>
  </div>
</form>

<div class="one-comment gbBlock gcBorder2">
{include file="gallery:modules/comment/templates/Comment.tpl"
	 item=$DeleteComment.item comment=$DeleteComment.comment
         user=$DeleteComment.commenter can=$DeleteComment.can}
</div>
