{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div id="gsContent" class="gcBorder1">
  <div class="gbBlock gcBackground1">
    <h2> {g->text text="Image Frame Samples"} </h2>
  </div>

  <div>
  {foreach from=$ImageFrameSample.list key=frame item=name}
    <div style="float:left;margin:6px">
    {$name}<br/>
    {if isset($ImageFrameSample.item)}
      {g->container type="imageframe.ImageFrame" frame=$frame
	  width=$ImageFrameSample.thumb.width height=$ImageFrameSample.thumb.height maxSize=150}
	{g->image item=$ImageFrameSample.item image=$ImageFrameSample.thumb
		  maxSize=150 id="%ID%" class="%CLASS%"}
      {/g->container}
    {else}
      {g->container type="imageframe.ImageFrame" frame=$frame width=150 height=126}
	<img src="{g->url href="modules/imageframe/images/sample.jpg"}"
	     width="150" height="126" id="%ID%" class="%CLASS%"/>
      {/g->container}
    {/if}
    </div>
  {/foreach}
  </div>
  <div class="gbBlock" style="clear: both">&nbsp;</div>
</div>
