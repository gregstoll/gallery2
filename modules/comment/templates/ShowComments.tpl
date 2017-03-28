{*
 * $Revision: 17586 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
{include file="gallery:modules/comment/templates/ChangeComment.js.tpl"}

<div class="gbBlock gcBackground1">
  <h2> {g->text text="View Comments"} </h2>
</div>

{if !empty($status)}
<div class="gbBlock">
  <h2 class="giSuccess">
    {if isset($status.changed)}
    {g->text text="Comment changed successfully"}
    {/if}
  </h2>
</div>
{/if}

{if empty($ShowComments.comments)}
<div class="gbBlock">
  <h3> {g->text text="There are no comments for this item"} </h3>
</div>
{else}
<div class="gbBlock">
{foreach from=$ShowComments.comments item=comment}
  <div id="comment-{$comment.randomId}" class="one-comment gcBorder2">
  {include file="gallery:modules/comment/templates/Comment.tpl"
	   comment=$comment item=$ShowComments.item can=$ShowComments.can
	   user=$ShowComments.commenters[$comment.commenterId]
           ajaxChangeCallback="changeComment" truncate=1024}
  </div>
{/foreach}
</div>
{/if}
