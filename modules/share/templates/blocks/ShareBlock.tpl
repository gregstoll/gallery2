{*
 * $Revision: 1909 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
{g->callback type="share.ShareBlockParams"}
{if !($block.share.onlyWhenLoggedIn && $user.isGuest)}
    <div class="{$class}">
        {* Add this link *}
	<!-- AddThis Button BEGIN -->
	<div class="addthis_toolbox addthis_default_style addthis_32x32_style">
	    <span class="sr-only">{g->text text="Share:"}</span>
	    <a class="addthis_button_facebook"></a>
	    <a class="addthis_button_twitter"></a>
	    <a class="addthis_button_email"></a>
	    <a class="addthis_button_favorites"></a>
	    <a class="addthis_button_print"></a>
	    <span class="addthis_separator">|</span>
	    <a href="http://www.addthis.com/bookmark.php?v=300&amp;pubid={$block.share.addThisAccountId}"
	       class="addthis_button_expanded">{g->text text="More"}</a>
	</div>
	<script type="text/javascript"
	        src="//s7.addthis.com/js/300/addthis_widget.js#pubid={$block.share.addThisAccountId}"
	        async="async"></script>
	<!-- AddThis Button END -->

	{* End Add this link *}
    </div>
{/if}



