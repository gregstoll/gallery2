{*
 * $Revision: 17477 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
{include file="gallery:modules/comment/templates/ChangeComment.js.tpl"}

<div class="gbBlock gcBackground1">
  <h2> {g->text text="Comments Awaiting Moderation"} </h2>
</div>

{if isset($status.allSpamDeleted)}
<div class="gbBlock"><h2 class="giSuccess">
  {g->text text="All spam comments deleted."}
</h2></div>
{/if}

{if empty($AdminModerateSpam.comments)}
<div class="gbBlock">
  <h3> {g->text text="There are no comments awaiting moderation."} </h3>
</div>
{else}
<div class="gbBlock">
  <h3>
{*
This message will be out of date as soon as we despam or delete a comment
via Ajax, so if we're going to use it, we should refresh it with the latest
count.  I fear that may be a bit expensive so I'm leaving it out for now.

    {g->text one="There is %d spam comment in the queue." many="There are %d spam comments in the queue." count=$AdminModerateSpam.total arg1=$AdminModerateSpam.total}
*}

    {g->text text="You can delete all spam comments at once, but check them carefully because you can't restore deleted comments."}
  </h3>
</div>
<div class="gbBlock gcBackground1">
  <input type="submit" class="inputTypeSubmit" name="{g->formVar var="form[action][deleteAllSpam]"}" value="{g->text text="Delete All Spam"}">
</div>

{if $AdminModerateSpam.navigator.pageCount > 1}
  {g->block type="core.Navigator" class="commentNavigator"
	    navigator=$AdminModerateSpam.navigator
	    currentPage=$AdminModerateSpam.navigator.page
	    totalPages=$AdminModerateSpam.navigator.pageCount}
{/if}
<table>
{foreach from=$AdminModerateSpam.comments item=comment}
<tr id="comment-{$comment.id}"><td style="text-align: center; padding: 0 4px">
  {assign var="item" value=$AdminModerateSpam.itemData[$comment.parentId]}
  <a id="CommentThumb-{$item.id}" href="{g->url arg1="view=core.ShowItem" arg2="itemId=`$item.id`"}">
    {if isset($item.thumb)}
      {g->image item=$item image=$item.thumb maxSize=120}
    {else}
      {$item.title|default:$item.pathComponent|markup}
    {/if}
  </a>
  <script type="text/javascript">
    //<![CDATA[
    {* force and alt/longdesc parameter here so that we avoid issues with single quotes in the title/description *}
    {if isset($item.resize)}
    new YAHOO.widget.Tooltip("gTooltip", {ldelim}
        context: "CommentThumb-{$item.id}", text: '{g->image item=$item image=$item.resize maxSize=500 alt="" longdesc=""}', showDelay: 250 {rdelim});
    {elseif isset($item.thumb)}
    new YAHOO.widget.Tooltip("gTooltip", {ldelim}
        context: "CommentThumb-{$item.id}", text: '{g->image item=$item image=$item.thumb alt="" longdesc=""}', showDelay: 250 {rdelim});
    {/if}
    //]]>
  </script>
</td><td>
  <div class="one-comment gcBorder2">
  {include file="gallery:modules/comment/templates/Comment.tpl"
	   comment=$comment can=$AdminModerateSpam.can[$comment.id]
	   item=$item user=$AdminModerateSpam.commenters[$comment.commenterId]
           ajaxChangeCallback="changeComment" truncate=1024}
  </div>
</td></tr>
{/foreach}
</table>
{/if}
