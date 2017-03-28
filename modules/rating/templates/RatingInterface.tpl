{*
 * $Revision: 17082 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
{if $RatingSummary.firstCall}
{include file="gallery:modules/rating/templates/RatingImagePreload.tpl"}
{/if}

<div class="giRatingUI">
	{foreach from=$RatingSummary.ratingValues item=ratingValue}{if $RatingData.canRate}<a
		href="javascript:rateItem({$RatingData.itemId}, {$ratingValue}, '{g->url
			arg1="view=rating.RatingCallback" arg2="command=rate"
			arg3="itemId=`$RatingData.itemId`" arg4="rating=$ratingValue"
			arg5="authToken=__AUTHTOKEN__"}')"
		onmouseover="updateStarDisplay({$RatingData.itemId}, {$ratingValue}); return true"
		onmouseout="resetStarDisplay({$RatingData.itemId}); return true">{/if}<img
		src="{g->url href="modules/rating/images/transparent.gif"}"
		id="rating.star.{$RatingData.itemId}.{$ratingValue}" class="giRatingUnit" alt=""
		title="{g->text text="Click a star to rate this item!"}"/>{if $RatingData.canRate}</a>{/if}{/foreach}
	<div class="giRatingAverageContainer">
		<div class="giRatingAverage" id="rating.averagePercent.{$RatingData.itemId}"
			style="width:{$RatingData.averagePercent}%"></div></div>
	<div class="giRatingVotes"><span
		id="rating.votes.{$RatingData.itemId}">{g->text
		one="%s vote" many="%s votes" count=$RatingData.votes arg1=$RatingData.votes}</span>
	</div>
	<span class="giRatingHidden"
		id="rating.rating.{$RatingData.itemId}">{$RatingData.rating}</span>
	<span class="giRatingHidden"
		id="rating.userRating.{$RatingData.itemId}">{$RatingData.userRating}</span>
</div>

<script type="text/javascript">
// <![CDATA[
resetStarDisplay({$RatingData.itemId});
// ]]>
</script>
