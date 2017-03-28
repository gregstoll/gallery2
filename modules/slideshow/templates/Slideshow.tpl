{*
 * $Revision: 17539 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
{foreach from=$Slideshow.items item=i name=items}
<a style="display: none" href="{g->url arg1="view=core.DownloadItem" arg2="itemId=`$i.image.id`" arg3="serialNumber=`$i.image.serialNumber`"}" rel="lyteshow[s]" title="{$i.item.title|markup|escape:html} {$i.item.summary|markup|escape:html}" id="a{$smarty.foreach.items.index}">.</a>
{/foreach}

<script type="text/javascript">
function start(startElId) {ldelim}
  {if !isset($Slideshow.piclensVersion)}
  startLB(startElId);
  {else}
  var p=PicLensLite;
  p.setCallbacks({ldelim} onNoPlugins:function(){ldelim}startLB(startElId){rdelim},
                          onExit:function(){ldelim}location.href='{$Slideshow.returnUrl}' {rdelim}
                 {rdelim});
  p.setLiteURLs({ldelim} swf:'{$Slideshow.piclensSwfUrl}' {rdelim});
  p.start({ldelim} feedUrl:'{$Slideshow.mediaRssUrl}',
                   guid:{$Slideshow.startItemId},
                   pid:'2PWfB4lurT4g',
                   delay:10
          {rdelim});
  {/if}
{rdelim}

{* PiclensLite already defines function startLytebox. Avoid that name. *}
{literal}
function startLB(startElId) {
  if (typeof myLytebox != 'undefined') {
    myLytebox.slideInterval = 10000;
    myLytebox.resizeSpeed = 10;
    myLytebox.start(document.getElementById(startElId), true, false);
    setTimeout('goBackOnStop()', 1000);
  } else {
    setTimeout('startLB("' + startElId + '")', 1000);
  }
}
function goBackOnStop() {
  var el = document.getElementById('lbOverlay');
  if (el && el.style.display != 'none') {
    setTimeout('goBackOnStop()', 1000);
  } else {
    history.go(-1);
  }
}
{/literal}
YAHOO.util.Event.addListener(window, 'load', function() {ldelim} start("a{$Slideshow.start}"); {rdelim}, false);
</script>
