{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
{* Set defaults *}
{if empty($item)} {assign var=item value=$theme.item} {/if}

{g->callback type="rating.LoadRating" itemId=$item.id}

{if !empty($block.rating.RatingData)}
<div class="{$class}">
{include file="gallery:modules/rating/templates/RatingInterface.tpl" 
	RatingData=$block.rating.RatingData
	RatingSummary=$block.rating.RatingSummary}
</div>
{/if}
