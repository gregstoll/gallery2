{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
{if !empty($WatermarkOption.watermarks)}
<script type="text/javascript">
  // <![CDATA[
  WatermarkOption_watermarkUrlMap = new Array;
  {foreach from=$WatermarkOption.watermarks item=watermark}
  WatermarkOption_watermarkUrlMap[{$watermark.id}] = new Array;
  WatermarkOption_watermarkUrlMap[{$watermark.id}]['url'] = '{g->url htmlEntities=false
    arg1="view=core.DownloadItem" arg2="itemId=`$watermark.id`"}';
  WatermarkOption_watermarkUrlMap[{$watermark.id}]['width'] = {$watermark.width};
  WatermarkOption_watermarkUrlMap[{$watermark.id}]['height'] = {$watermark.height};
  {/foreach}

  {literal}
  function WatermarkOption_chooseWatermark(id) {
    var watermark = document.getElementById('WatermarkOption_watermark');
    if (id) {
      watermark.src = WatermarkOption_watermarkUrlMap[id]['url'];
      watermark.width = WatermarkOption_watermarkUrlMap[id]['width'];
      watermark.height = WatermarkOption_watermarkUrlMap[id]['height'];
      watermark.style.display = 'block';
    } else {
      watermark.style.display = 'none';
    }
  }
  // ]]>
  {/literal}
</script>

<div class="gbBlock">
  <img id="WatermarkOption_watermark" src="" alt="" width="0" height="0"
       style="display: none; float: right; padding: 10px 30px 0"/>

  <h3> {g->text text="Watermark"} </h3>

  <p class="giDescription">
    {g->text text="Choose a watermark to apply to the images you add."}
    <br/>
    <a href="{g->url arg1="view=core.UserAdmin" arg2="subView=watermark.UserWatermarks"}">
      {g->text text="Edit your watermarks"}
    </a>
    <br/>
    <select style="margin: 5px 0 5px 3px"
     name="{g->formVar var="form[WatermarkOption][watermarkId]"}"
     onchange="WatermarkOption_chooseWatermark(this.value)">
      <option value="">&laquo; {g->text text="none"} &raquo;</option>
      {foreach from=$WatermarkOption.watermarks item=watermark}
	<option value="{$watermark.id}">{$watermark.name}</option>
      {/foreach}
    </select>
  </p>
  <div style="clear: both"></div>
</div>
{/if}
