{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="{$class}">
  {if !empty($showDate)}
  <div class="date summary">
    {capture name=childTimestamp}{g->date timestamp=$item.originationTimestamp}{/capture}
    {g->text text="Date: %s" arg1=$smarty.capture.childTimestamp}
  </div>
  {/if}

  {if !empty($showOwner)}
  <div class="owner summary">
    {g->text text="Owner: %s" arg1=$item.owner.fullName|default:$item.owner.userName}
  </div>
  {/if}

  {if !empty($showSize) && $item.canContainChildren && $item.childCount > 0}
  <div class="size summary">
    {g->text one="Size: %d item"
	     many="Size: %d items"
	     count=$item.childCount
	     arg1=$item.childCount}
    {if $item.descendentCount > $item.childCount}
    {g->text one="(%d item total)"
	     many="(%d items total)"
	     count=$item.descendentCount
	     arg1=$item.descendentCount}
    {/if}
  </div>
  {/if}

  {if !empty($showViewCount) && $item.viewCount > 0}
  <div class="viewCount summary">
    {g->text text="Views: %d" arg1=$item.viewCount}
  </div>
  {/if}

  {if !empty($showSummaries)}
  {foreach from=$item.itemSummaries key=name item=summary}
  <div class="summary-{$name} summary">
    {$summary}
  </div>
  {/foreach}
  {/if}
</div>
