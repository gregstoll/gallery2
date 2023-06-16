{*
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
{g->callback type="addtoany.AddToAnyBlockParams"}
{if !($block.addtoany.onlyWhenLoggedIn && $user.isGuest)}
    <div class="{$class}">
	<!-- AddToAny Button BEGIN -->
	    <span class="sr-only">{g->text text="Share:"}</span>
	<div class="a2a_kit a2a_default_style a2a_kit_size_32">
	    <a class="a2a_button_facebook"></a>
	    <a class="a2a_button_twitter"></a>
	    <a class="a2a_button_pinterest"></a>
	    <a class="a2a_button_print"></a>
        <a class="a2a_dd" href="https://www.addtoany.com/share"></a>
	</div>
        <script type="text/javascript"
                src="https://static.addtoany.com/menu/page.js"
	        async="async"></script>
	<!-- AddToAny Button END -->
    <div style="clear: both;"></div>
    </div>
{/if}



