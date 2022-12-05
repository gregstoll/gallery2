{*
 * $Revision: 996 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
{if empty($width)} {assign var=width value=85}{/if}
{if !isset($showHeadings)} {assign var=showHeadings value=1}{/if}
{assign var=itemTitle value=$GetUrlsData.itemTitle|markup:strip|escape}
{if empty($isBlock)}
<div class="gbBlock gcBackground1">
  <h2> 	{g->text text="Quick URLs"} </h2>
</div>
{assign var=isBlock value=0}
{elseif !empty($blockToggle)}
  <h3>
    {g->text text="Show URLs"}
    <span id="GetUrls_{$GetUrlsData.itemId}-toggle"
     class="giBlockToggle gcBackground1 gcBorder2" style="border-width: 1px"
     onclick="BlockToggle('GetUrls_{$GetUrlsData.itemId}_details', 
             'GetUrls_{$GetUrlsData.itemId}-toggle')">+</span>
  </h3>
{/if}
<div id="GetUrls_{$GetUrlsData.itemId}_details"{if !empty($blockToggle)} style="display: none;"{/if}>
{if $GetUrlsData.guestMode && $GetUrlsData.isPhoto && !$GetUrlsData.permissions.viewSource && 
      !$GetUrlsData.permissions.viewResizes && ($isBlock && $showWarnings || !$isBlock && !$GetUrlsData.options.suppressSourceWarning)}
<div class="gbBlock">
{assign var=divOpen value=1}
  <h3 class="giError">
    {g->text text="Warning! The permissions for this item are set so that unauthenticated users may not view it. If you are planning on linking to this item in a public forum or HTML page, the viewers will most likely not be able to access it."}
  </h3>
{/if}

{if $GetUrlsData.guestMode && $GetUrlsData.isPhoto && !$GetUrlsData.permissions.viewSource && 
      $GetUrlsData.permissions.viewResizes && ($isBlock && $showWarnings || !$isBlock && !$GetUrlsData.options.suppressResizedWarning)}
{if !isset($divOpen)}
<div class="gbBlock">
{assign var=divOpen value=1}
{/if}
  <h3 class="giError">
    {g->text text="Warning! The permissions for this item are set so that unauthenticated users may not view the full sized version, but the resized version is available. Links to the resized version are being used, this may not be what you want."}
  </h3>
{/if}

{if !$isBlock}
{if !isset($divOpen)}
<div class="gbBlock">
{/if}
  <p class="gbDescription">
    {g->text text="Through this page you can quickly get formatted URLs for this item."}
    <br/>
    {g->text text="These can be used to post the item/image in forums and HTML pages by copying the code in the boxes below and pasting it into your application/browser."}
  </p>
{/if}
{if isset($divOpen)}
</div>
{/if}

{if $GetUrlsData.isPhoto && $GetUrlsData.permissions.viewSource && $GetUrlsData.permissions.viewResizes && 
      $GetUrlsData.options.HtmlResize2Full && isset($GetUrlsData.resizeId)}
{assign var=showHtmlResize2Full value=1}
{else}
{assign var=showHtmlResize2Full value=0}
{/if}

{if $GetUrlsData.isPhoto && $GetUrlsData.options.HtmlInline && 
    ($GetUrlsData.permissions.viewSource || ($GetUrlsData.permissions.viewResizes && isset($GetUrlsData.resizeId)))}
{assign var=showHtmlInline value=1}
{else}
{assign var=showHtmlInline value=0}
{/if}

{if $GetUrlsData.options.HtmlThumbnail && isset($GetUrlsData.thumbId)}
{assign var=showHtmlThumbnail value=1}
{else}
{assign var=showHtmlThumbnail value=0}
{/if}

{if $GetUrlsData.options.HtmlLink || $showHtmlInline || $showHtmlThumbnail || $showHtmlResize2Full}
<div class="gbBlock">
  {if $showHeadings}
  <h3>{g->text text="HTML Formatted Links"}</h3>
  {/if}
  {if $GetUrlsData.options.HtmlLink}
  <p class="gbDescription">
    {g->text text="Link to the page of the item:"}
    <br />
    <input onclick="this.focus(); this.select();" name="forum" type="text" readonly="true" size="{$width}" value="&lt;a href=&quot;{g->url arg1="view=core.ShowItem" arg2="itemId=`$GetUrlsData.itemId`" forceFullUrl=true forceSessionId=false}&quot;&gt;click for image&lt;/a&gt;" />
    {if $GetUrlsData.showIeLinks} <a onclick="javascript:navigator.clipboard.writeText('&lt;a href=&quot;{g->url arg1="view=core.ShowItem" arg2="itemId=`$GetUrlsData.itemId`" forceFullUrl=true forceSessionId=false htmlEntities=false}&quot;&gt;click for image&lt;/a&gt;');window.status = '[CLK]{$GetUrlsData.itemId}[/CLK] copied to your clipboard for forum posting!';" href>[CLK]</a>{/if}
  </p>
  {/if}
  
  {if $showHtmlInline}
  {if $GetUrlsData.permissions.viewSource}
  <p class="gbDescription">
    {g->text text="Show image directly:"}
    <br />
    <input onclick="this.focus(); this.select();" name="forum" type="text" readonly="true" size="{$width}" value="&lt;img src=&quot;{g->url arg1="view=core.DownloadItem" arg2="itemId=`$GetUrlsData.itemId`" forceFullUrl=true forceSessionId=false}&quot; alt=&quot;{$itemTitle}&quot;&gt;">
    {if $GetUrlsData.showIeLinks} <a onclick="javascript:navigator.clipboard.writeText('&lt;img src=&quot;{g->url arg1="view=core.DownloadItem" arg2="itemId=`$GetUrlsData.itemId`" forceFullUrl=true forceSessionId=false htmlEntities=false}&quot; alt=&quot;{$itemTitle}&quot;&gt;');window.status = '[IMG]{$GetUrlsData.itemId}[/IMG] copied to your clipboard for forum posting!';" href>[IMG]</a>{/if}
  </p>
  {elseif $GetUrlsData.permissions.viewResizes && isset($GetUrlsData.resizeId)}
  <p class="gbDescription">
    {g->text text="Show image directly (resized version will be shown):"}
    <br />
    <input onclick="this.focus(); this.select();" name="forum" type="text" readonly="true" size="{$width}" value="&lt;img src=&quot;{g->url arg1="view=core.DownloadItem" arg2="itemId=`$GetUrlsData.resizeId`" forceFullUrl=true forceSessionId=false}&quot; alt=&quot;{$itemTitle}&quot;&gt;">
    {if $GetUrlsData.showIeLinks} <a onclick="javascript:navigator.clipboard.writeText('&lt;img src=&quot;{g->url arg1="view=core.DownloadItem" arg2="itemId=`$GetUrlsData.resizeId`" forceFullUrl=true forceSessionId=false htmlEntities=false}&quot; alt=&quot;{$itemTitle}&quot;&gt;');window.status = '[IMG]{$GetUrlsData.itemId}[/IMG] copied to your clipboard for forum posting!';" href>[IMG]</a>{/if}
  </p>
  {/if}
  {/if}

  {if $showHtmlResize2Full}
  <p class="gbDescription">
    {g->text text="Show clickable resized image linking to original:"}
    <br />
    <input onclick="this.focus(); this.select();" name="forum" type="text" readonly="true" size="{$width}" value="&lt;a href=&quot;{g->url arg1="view=core.DownloadItem" arg2="itemId=`$GetUrlsData.itemId`" forceFullUrl=true forceSessionId=false}&quot;&gt;&lt;img src=&quot;{g->url arg1="view=core.DownloadItem" arg2="itemId=`$GetUrlsData.resizeId`" forceFullUrl=true forceSessionId=false}&quot; alt=&quot;{$itemTitle}&quot;&gt;&lt;/a&gt;">
    {if $GetUrlsData.showIeLinks} <a onclick="javascript:navigator.clipboard.writeText('&lt;a href=&quot;{g->url arg1="view=core.DownloadItem" arg2="itemId=`$GetUrlsData.itemId`" forceFullUrl=true forceSessionId=false htmlEntities=false}&quot;&gt;&lt;img src=&quot;{g->url arg1="view=core.DownloadItem" arg2="itemId=`$GetUrlsData.resizeId`" forceFullUrl=true forceSessionId=false}&quot; alt=&quot;{$itemTitle}&quot;&gt;&lt;/a&gt;');window.status = '[R2F]{$GetUrlsData.resizeId}[/R2F] copied to your clipboard for forum posting!';" href>[R2F]</a>{/if}
  </p>
  {/if}

  {if $showHtmlThumbnail}
  <p class="gbDescription">
    {g->text text="Show a clickable thumbnail linking to the item:"}
    <br />
    <input onclick="this.focus(); this.select();" name="forum" type="text" readonly="true" size="{$width}" value="&lt;a href=&quot;{g->url arg1="view=core.ShowItem" arg2="itemId=`$GetUrlsData.itemId`" forceFullUrl=true forceSessionId=false}&quot;&gt;&lt;img src=&quot;{g->url arg1="view=core.DownloadItem" arg2="itemId=`$GetUrlsData.thumbId`" forceFullUrl=true forceSessionId=false}&quot; alt=&quot;{$itemTitle}&quot;&gt;&lt;/a&gt;">
    {if $GetUrlsData.showIeLinks} <a onclick="javascript:navigator.clipboard.writeText('&lt;a href=&quot;{g->url arg1="view=core.ShowItem" arg2="itemId=`$GetUrlsData.itemId`" forceFullUrl=true forceSessionId=false htmlEntities=false}&quot;&gt;&lt;img src=&quot;{g->url arg1="view=core.DownloadItem" arg2="itemId=`$GetUrlsData.thumbId`" forceFullUrl=true forceSessionId=false}&quot; alt=&quot;{$itemTitle}&quot;&gt;&lt;/a&gt;');window.status = '[TMB]{$GetUrlsData.thumbId}[/TMB] copied to your clipboard for forum posting!';" href>[TMB]</a>{/if}
  </p>
  {/if}
</div>
{/if}

{if $GetUrlsData.isPhoto && $GetUrlsData.permissions.viewSource && $GetUrlsData.permissions.viewResizes && 
      $GetUrlsData.options.BbResize2Full && isset($GetUrlsData.resizeId)}
{assign var=showBbResize2Full value=1}
{else}
{assign var=showBbResize2Full value=0}
{/if}

{if $GetUrlsData.isPhoto && $GetUrlsData.options.BbInline && 
    ($GetUrlsData.permissions.viewSource || ($GetUrlsData.permissions.viewResizes && isset($GetUrlsData.resizeId)))}
{assign var=showBbInline value=1}
{else}
{assign var=showBbInline value=0}
{/if}

{if $GetUrlsData.options.BbThumbnail && isset($GetUrlsData.thumbId)}
{assign var=showBbThumbnail value=1}
{else}
{assign var=showBbThumbnail value=0}
{/if}

{if $GetUrlsData.options.BbLink || $showBbInline || $showBbThumbnail || $showBbResize2Full}
<div class="gbBlock">
  {if $showHeadings}
  <h3>{g->text text="BBCode Formatted Links"}</h3>
  {/if}
  {if $GetUrlsData.options.BbLink}
  <p class="gbDescription">
    {g->text text="Link to the page of the item:"}
    <br />
    <input onclick="this.focus(); this.select();" name="forum" type="text" readonly="true" size="{$width}" value="[url={g->url arg1="view=core.ShowItem" arg2="itemId=`$GetUrlsData.itemId`" forceFullUrl=true forceSessionId=false}]click for image[/url]">
    {if $GetUrlsData.showIeLinks} <a onclick="javascript:navigator.clipboard.writeText('[url={g->url arg1="view=core.ShowItem" arg2="itemId=`$GetUrlsData.itemId`" forceFullUrl=true forceSessionId=false htmlEntities=false}]click for image[/url]');window.status = '[CLK]{$GetUrlsData.itemId}[/CLK] copied to your clipboard for forum posting!';" href>[CLK]</a>{/if}
  </p>
  {/if}

  {if $showBbInline}
  {if $GetUrlsData.permissions.viewSource}
  <p class="gbDescription">
    {g->text text="Show image directly:"}
    <br />
    <input onclick="this.focus(); this.select();" name="forum" type="text" readonly="true" size="{$width}" value="[img]{g->url arg1="view=core.DownloadItem" arg2="itemId=`$GetUrlsData.itemId`" forceFullUrl=true forceSessionId=false}[/img]">
    {if $GetUrlsData.showIeLinks} <a onclick="javascript:navigator.clipboard.writeText('[IMG]{g->url arg1="view=core.DownloadItem" arg2="itemId=`$GetUrlsData.itemId`" forceFullUrl=true forceSessionId=false htmlEntities=false}[/IMG]');window.status = '[IMG]{$GetUrlsData.itemId}[/IMG] copied to your clipboard for forum posting!';" href>[IMG]</a>{/if}
  </p>
  {elseif $GetUrlsData.permissions.viewResizes && isset($GetUrlsData.resizeId)}
  <p class="gbDescription">
    {g->text text="Show image directly (resized version will be shown):"}
    <br />
    <input onclick="this.focus(); this.select();" name="forum" type="text" readonly="true" size="{$width}" value="[img]{g->url arg1="view=core.DownloadItem" arg2="itemId=`$GetUrlsData.resizeId`" forceFullUrl=true forceSessionId=false}[/img]">
    {if $GetUrlsData.showIeLinks} <a onclick="javascript:navigator.clipboard.writeText('[IMG]{g->url arg1="view=core.DownloadItem" arg2="itemId=`$GetUrlsData.resizeId`" forceFullUrl=true forceSessionId=false htmlEntities=false}[/IMG]');window.status = '[IMG]{$GetUrlsData.resizeId}[/IMG] copied to your clipboard for forum posting!';" href>[IMG]</a>{/if}   </p>
  </p>
  {/if}
  {/if}

  {if $showBbResize2Full}
  <p class="gbDescription">
    {g->text text="Show a clickable resized image linking to the original:"}
    <br />
    <input onclick="this.focus(); this.select();" name="forum" type="text" readonly="true" size="{$width}" value="[url={g->url arg1="view=core.DownloadItem" arg2="itemId=`$GetUrlsData.itemId`" forceFullUrl=true forceSessionId=false}][img]{g->url arg1="view=core.DownloadItem" arg2="itemId=`$GetUrlsData.resizeId`" forceFullUrl=true forceSessionId=false}[/img][/url]">
    {if $GetUrlsData.showIeLinks} <a onclick="javascript:navigator.clipboard.writeText('[url={g->url arg1="view=core.DownloadItem" arg2="itemId=`$GetUrlsData.itemId`" forceFullUrl=true forceSessionId=false htmlEntities=false}][img]{g->url arg1="view=core.DownloadItem" arg2="itemId=`$GetUrlsData.resizeId`" forceFullUrl=true forceSessionId=false}[/img][/url]');window.status = '[R2F]{$GetUrlsData.resizeId}[/R2F] copied to your clipboard for forum posting!';" href>[R2F]</a>{/if}
  </p>
  {/if}

  {if $showBbThumbnail}
  <p class="gbDescription">
    {g->text text="Show a clickable thumbnail linking to the original:"}
    <br />
    <input onclick="this.focus(); this.select();" name="forum" type="text" readonly="true" size="{$width}" value="[url={g->url arg1="view=core.ShowItem" arg2="itemId=`$GetUrlsData.itemId`" forceFullUrl=true forceSessionId=false}][img]{g->url arg1="view=core.DownloadItem" arg2="itemId=`$GetUrlsData.thumbId`" forceFullUrl=true forceSessionId=false}[/img][/url]">
    {if $GetUrlsData.showIeLinks} <a onclick="javascript:navigator.clipboard.writeText('[url={g->url arg1="view=core.ShowItem" arg2="itemId=`$GetUrlsData.itemId`" forceFullUrl=true forceSessionId=false htmlEntities=false}][img]{g->url arg1="view=core.DownloadItem" arg2="itemId=`$GetUrlsData.thumbId`" forceFullUrl=true forceSessionId=false}[/img][/url]');window.status = '[TMB]{$GetUrlsData.thumbId}[/TMB] copied to your clipboard for forum posting!';" href>[TMB]</a>{/if}
  </p>
  {/if}
</div>
{/if}

{if $GetUrlsData.options.MiscItemId || $GetUrlsData.options.MiscThumbnailId || $GetUrlsData.options.MiscResizeId}
<div class="gbBlock">
  {if $showHeadings}
  <h3>{g->text text="Misc."}</h3>
  {/if}
  {if $GetUrlsData.options.MiscItemId}
  <p class="gbDescription">
    {g->text text="The ID for this item is %s" arg1=$GetUrlsData.itemId}
  </p>
  {if $GetUrlsData.isDataItem && $GetUrlsData.permissions.viewSource}
  <p class="gbDescription">
    {g->text text="Download URL for original item:"}
    <br />
    <input onclick="this.focus(); this.select();" name="forum" type="text" readonly="true" size="{$width}" value="{g->url arg1="view=core.DownloadItem" arg2="itemId=`$GetUrlsData.itemId`" forceFullUrl=true forceSessionId=false}">
    {if $GetUrlsData.showIeLinks} <a onclick="javascript:navigator.clipboard.writeText('{g->url arg1="view=core.DownloadItem" arg2="itemId=`$GetUrlsData.itemId`" forceFullUrl=true forceSessionId=false htmlEntities=false}');window.status = 'http link to Full Item ID {$GetUrlsData.itemId} copied to your clipboard!';" href>[IMG]</a>{/if}   </p>
  </p>
  {/if}
  {/if}

  {if $GetUrlsData.isPhoto && isset($GetUrlsData.resizeId) && $GetUrlsData.options.MiscResizeId}
  <p class="gbDescription">
    {g->text text="The ID of the resized version that we're using is %s," arg1=$GetUrlsData.resizeId}
  </p>
  <p class="gbDescription">
    {g->text text="URL for resized version:"}
    <br />
    <input onclick="this.focus(); this.select();" name="forum" type="text" readonly="true" size="{$width}" value="{g->url arg1="view=core.DownloadItem" arg2="itemId=`$GetUrlsData.resizeId`" forceFullUrl=true forceSessionId=false}">
    {if $GetUrlsData.showIeLinks} <a onclick="javascript:navigator.clipboard.writeText('{g->url arg1="view=core.DownloadItem" arg2="itemId=`$GetUrlsData.resizeId`" forceFullUrl=true forceSessionId=false htmlEntities=false}');window.status = 'http link to Resized Item ID {$GetUrlsData.resizeId} copied to your clipboard!';" href>[IMG]</a>{/if}   </p>
  </p>
  {/if}

  {if $GetUrlsData.options.MiscThumbnailId && isset($GetUrlsData.thumbId)}
  <p class="gbDescription">
    {g->text text="The ID of the thumbnail is %s." arg1=$GetUrlsData.thumbId}
  </p>
  <p class="gbDescription">
    {g->text text="URL for thumbnail:"}
    <br />
    <input onclick="this.focus(); this.select();" name="forum" type="text" readonly="true" size="{$width}" value="{g->url arg1="view=core.DownloadItem" arg2="itemId=`$GetUrlsData.thumbId`" forceFullUrl=true forceSessionId=false}">
    {if $GetUrlsData.showIeLinks} <a onclick="javascript:navigator.clipboard.writeText('{g->url arg1="view=core.DownloadItem" arg2="itemId=`$GetUrlsData.thumbId`" forceFullUrl=true forceSessionId=false htmlEntities=false}');window.status = 'http link to Thumb Item ID {$GetUrlsData.thumbId} copied to your clipboard!';" href>[IMG]</a>{/if}   </p>
  </p>
  {/if}
</div>
{/if}
</div>
