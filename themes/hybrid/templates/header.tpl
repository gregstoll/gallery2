{*
 * $Revision: 16849 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<script type="text/javascript">
// <![CDATA[
{if $theme.imageCount==1}
var data_iw = new Array(1); data_iw[0] = {$theme.imageWidths};
var data_ih = new Array(1); data_ih[0] = {$theme.imageHeights};
{else}
var data_iw = new Array({$theme.imageWidths});
var data_ih = new Array({$theme.imageHeights});
{/if}
var data_count = data_iw.length, data_name = '{$theme.item.id}',
    data_view = {$theme.viewIndex|default:-1}, cookie_path = '{$theme.cookiePath}',
    prev_page = '{if $theme.currentPage > 1}{strip}
      {g->url params=$theme.pageUrl arg1="page=`$theme.currentPage-1`" arg2="show=last"
	      arg3="slide=__SLIDE__" htmlEntities=false}
    {/strip}{/if}',
    next_page = '{if $theme.currentPage < $theme.totalPages}{strip}
      {g->url params=$theme.pageUrl arg1="page=`$theme.currentPage+1`" arg2="show=first"
	      arg3="slide=__SLIDE__" htmlEntities=false}
    {/strip}{/if}',
    album_showtext = '{g->text text="Show Details" forJavascript=true}',
    album_hidetext = '{g->text text="Hide Details" forJavascript=true}',
    album_showlinks = '{g->text text="Show Item Links" forJavascript=true}',
    album_hidelinks = '{g->text text="Hide Item Links" forJavascript=true}',
    item_details = '{g->text text="Item Details" forJavascript=true}',
    keyboard_help = "{g->text text="Album view:\n| space = start slideshow\n| ctrl-right/left = show/hide sidebar\n| ctrl-up/down = show/hide item links\nImage view:\n| space = start/pause slideshow\n| escape = return to album view\n| left/right = next/prev image\n| up/down = show hide description text\n| page up/down = scroll description text\n| ctrl-up/down = select full/fit size\nArrow keys scroll image in full size; use shift-arrows for funcs above\nItem Details showing:\n| escape = close popup" forJavascript=true}";
// ]]>
</script>
<script type="text/javascript" src="{$theme.themeUrl}/hybrid.js"></script>
<style type="text/css">
#gsAlbumContent td.t {ldelim} width: {$theme.columnWidthPct}%; {rdelim}
</style>
