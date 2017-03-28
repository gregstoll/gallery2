{*
 * $Revision: 17673 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
{if !empty($comment.subject)}
<h3>
  {$comment.subject|markup}
</h3>
{/if}

{assign var="needSeparator" value="false"}
{if $can.edit}
<span class="edit">
  <a href="{g->url arg1="view=comment.EditComment" arg2="itemId=`$item.id`" arg3="commentId=`$comment.id`" arg4="return=true"}">{g->text text="edit"}</a>
</span>
{assign var="needSeparator" value="true"}
{/if}

{if $can.delete}
{if $needSeparator}|{/if}
<span class="delete">
  <a{if !empty($ajaxChangeCallback)} onclick="{$ajaxChangeCallback}('{$comment.randomId}', 'delete'); return false;"{/if} href="{g->url arg1="view=comment.DeleteComment" arg2="itemId=`$item.id`" arg3="commentId=`$comment.id`" arg4="return=true"}">{g->text text="delete"}</a>
</span>
{assign var="needSeparator" value="true"}
{/if}

{if !empty($can.markNotSpam)}
{if $needSeparator}|{/if}
<span class="marknotspam">
  <a href="javascript:{$ajaxChangeCallback}('{$comment.randomId}', 'despam');">{g->text text="mark as not spam"}</a>
</span>
{assign var="needSeparator" value="true"}
{/if}

{if !empty($can.markSpam)}
{if $needSeparator}|{/if}
<span class="markSpam">
  <a href="javascript:{$ajaxChangeCallback}('{$comment.randomId}', 'spam');">{g->text text="mark as spam"}</a>
</span>
{assign var="needSeparator" value="true"}
{/if}

{assign var="commentText" value=$comment.comment|markup}
{if isset($truncate)}
  {assign var="truncated" value=$commentText|entitytruncate:$truncate}
{/if}

{if isset($truncate) && ($truncated != $commentText)}
<span class="showFull">
  {if $needSeparator}| {/if}
  <a id="comment-more-toggle-{$comment.randomId}"
      onclick="document.getElementById('comment-truncated-{$comment.randomId}').style.display='none';
	       document.getElementById('comment-full-{$comment.randomId}').style.display='block';
	       document.getElementById('comment-more-toggle-{$comment.randomId}').style.display='none';
	       document.getElementById('comment-less-toggle-{$comment.randomId}').style.display='inline'; return false;"
      href="">{g->text text="show full"}</a><a id="comment-less-toggle-{$comment.randomId}"
      onclick="document.getElementById('comment-truncated-{$comment.randomId}').style.display='block';
	       document.getElementById('comment-full-{$comment.randomId}').style.display='none';
	       document.getElementById('comment-more-toggle-{$comment.randomId}').style.display='inline';
	       document.getElementById('comment-less-toggle-{$comment.randomId}').style.display='none'; return false;"
      href="" style="display: none">{g->text text="show summary"}</a>
</span>
  <p id="comment-truncated-{$comment.randomId}" class="comment">
    {$truncated}
  </p>
  <p id="comment-full-{$comment.randomId}" class="comment" style="display: none">
    {$commentText}
  </p>
{else}
  <p class="comment">
    {$commentText}
  </p>
{/if}

<p class="info">
  {capture name="date"}{g->date timestamp=$comment.date style="datetime"}{/capture}
  {if empty($comment.author)}
    {if $can.edit}
      {g->text text="Posted by %s on %s (%s)"
	       arg1=$user.fullName|default:$user.userName
	       arg2=$smarty.capture.date arg3=$comment.host}
    {else}
      {g->text text="Posted by %s on %s"
	       arg1=$user.fullName|default:$user.userName arg2=$smarty.capture.date}
    {/if}
  {else}
    {if $can.edit}
      {g->text text="Posted by %s (guest) on %s (%s)"
	       arg1=$comment.author|default:$user.userName
	       arg2=$smarty.capture.date arg3=$comment.host}
    {else}
      {g->text text="Posted by %s (guest) on %s"
	       arg1=$comment.author|default:$user.userName arg2=$smarty.capture.date}
    {/if}
  {/if}
</p>
