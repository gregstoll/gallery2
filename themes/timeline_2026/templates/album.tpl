{* timeline_2026 - album view: a date-grouped stream, or a grid of album cards *}

{* albums-only layout when every child is an album; otherwise the photo stream,
   which branches per child so a sub-album mixed among photos still navigates *}
{assign var="gridMode" value="albums"}
{foreach from=$theme.children item=probeChild}
  {if !$probeChild.canContainChildren}
    {assign var="gridMode" value="photos"}
  {/if}
{/foreach}

<div class="tl-head">
  {if !empty($theme.item.title)}
    <h1>{$theme.item.title|markup}</h1>
  {else}
    <h1>{g->text text="Photos"}</h1>
  {/if}
  {if !empty($theme.item.description)}
    <p class="tl-desc">{$theme.item.description|markup}</p>
  {/if}
  <div class="tl-meta">
    {if !empty($theme.item.descendentCount)}
      <span>{$theme.item.descendentCount} {g->text text="items"}</span>
    {/if}
    {g->block type="core.ItemInfo" item=$theme.item showDate=true showSize=true class="giInfo"}
  </div>
</div>

{if !count($theme.children)}
  <div class="tl-empty">
    <p>{g->text text="This album is empty."}</p>
    {if isset($theme.permissions.core_addDataItem)}
      <a class="tl-btn" href="{g->url arg1="view=core.ItemAdmin" arg2="subView=core.ItemAdd" arg3="itemId=`$theme.item.id`"}">
        {g->text text="Add items"}</a>
    {/if}
  </div>

{elseif $gridMode == 'albums'}
  {* ---------------- album cards ---------------- *}
  <div class="tl-albums">
    {if isset($theme.permissions.core_addAlbumItem)}
      <a class="tl-card tl-card--new"
         href="{g->url arg1="view=core.ItemAdmin" arg2="subView=core.ItemAddAlbum" arg3="itemId=`$theme.item.id`"}">
        <span class="tl-card-new-mark" aria-hidden="true">+</span>
        <span class="tl-card-title">{g->text text="Create album"}</span>
      </a>
    {/if}
    {foreach from=$theme.children item=child}
      {capture assign=linkUrl}{strip}
        {if $theme.params.dynamicLinks == 'jump'}
          {g->url arg1="view=core.ShowItem" arg2="itemId=`$child.id`"}
        {else}
          {g->url params=$theme.pageUrl arg1="itemId=`$child.id`"}
        {/if}
      {/strip}{/capture}
      <a class="tl-card" href="{$linkUrl}">
        <span class="tl-card-cover">
          {if isset($child.thumbnail)}
            {g->image item=$child image=$child.thumbnail class="giThumbnail"
                       loading="lazy" decoding="async"}
          {else}
            <span class="tl-noimg" aria-hidden="true"></span>
          {/if}
        </span>
        <span class="tl-card-title">
          {if !empty($child.title)}{$child.title|markup:strip}{else}{$child.pathComponent}{/if}
        </span>
        <span class="tl-card-sub">
          {if !empty($child.descendentCount)}{$child.descendentCount} {g->text text="items"}{/if}
          {if !empty($child.originationTimestamp)}
            &middot; {g->date timestamp=$child.originationTimestamp format="%Y"}
          {/if}
        </span>
      </a>
    {/foreach}
  </div>

{else}
  {* ---------------- date-grouped photo stream ----------------
     Every month gets a section so the scrubber can reach it, but only the
     first few carry tiles as markup. The rest are hydrated by timeline.js
     from the payload below when they come near the viewport. *}
  <div class="tl-stream" id="tlStream"
       data-thumb-tpl="{g->url arg1="view=core.DownloadItem" arg2="itemId=__TID__" arg3="serialNumber=__TSN__"}"
       data-full-tpl="{g->url arg1="view=core.DownloadItem" arg2="itemId=__ID__"}"
       data-link-tpl="{g->url params=$theme.pageUrl arg1="itemId=__ID__"}"
       data-album-label="{g->text text="Album"}">
  {foreach from=$theme.groups item=group}
    <section class="tl-group{if !$group.eager} is-lazy{/if}" data-key="{$group.key}"
             data-count="{$group.count}"
             {if !$group.eager}style="contain-intrinsic-size:auto {$group.est}px;"{/if}>
      <h2 class="tl-groupdate">{if $group.label}{$group.label}{else}{g->text text="Undated"}{/if}</h2>
      <div class="tl-just"{if !$group.eager} style="min-height:{$group.est}px"{/if}>
      {foreach from=$group.items item=child}
        {if !empty($child.width) && !empty($child.height)}
          {math assign="ar" equation="w / h" w=$child.width h=$child.height format="%.4f"}
        {elseif isset($child.thumbnail) && !empty($child.thumbnail.height)}
          {math assign="ar" equation="w / h" w=$child.thumbnail.width h=$child.thumbnail.height format="%.4f"}
        {else}
          {assign var="ar" value="1.3333"}
        {/if}
        {capture assign=linkUrl}{strip}
          {if $theme.params.dynamicLinks == 'jump'}
            {g->url arg1="view=core.ShowItem" arg2="itemId=`$child.id`"}
          {else}
            {g->url params=$theme.pageUrl arg1="itemId=`$child.id`"}
          {/if}
        {/strip}{/capture}
        <div class="tl-cell" style="--ar: {$ar};">
          {if $child.canContainChildren}
            <a class="tl-tile tl-tile--album" href="{$linkUrl}">
              {if isset($child.thumbnail)}
                {g->image item=$child image=$child.thumbnail class="giThumbnail"
                       loading="lazy" decoding="async"}
              {else}<span class="tl-noimg" aria-hidden="true"></span>{/if}
              <span class="tl-tile-cap">
                <span class="tl-badge">{g->text text="Album"}</span>
                {if !empty($child.title)}{$child.title|markup:strip}{else}{$child.pathComponent}{/if}</span>
            </a>
          {else}
            <a class="tl-tile" href="{$linkUrl}"
               data-full="{g->url arg1="view=core.DownloadItem" arg2="itemId=`$child.id`"}"
               data-title="{if !empty($child.title)}{$child.title|markup:strip|escape}{else}{$child.pathComponent|escape}{/if}"
               {if !empty($child.dateLabel)}data-date="{$child.dateLabel}"{/if}>
              {if isset($child.thumbnail)}
                {g->image item=$child image=$child.thumbnail class="giThumbnail"
                       loading="lazy" decoding="async"}
              {else}<span class="tl-noimg" aria-hidden="true"></span>{/if}
              <span class="tl-tile-cap">
                {if !empty($child.title)}{$child.title|markup:strip}{else}{$child.pathComponent}{/if}</span>
            </a>
          {/if}
        </div>
      {/foreach}
      </div>
    </section>
  {/foreach}
  </div>

  {if !empty($theme.lazyData)}
    <script type="application/json" id="tlLazyData">{$theme.lazyData}</script>
  {/if}

  {* rendered complete from the whole item set, so it does not grow as
     sections hydrate *}
  {if !empty($theme.yearMarks)}
    <nav class="tl-scrub" id="tlScrub" aria-label="{g->text text="Jump to year"}">
      {foreach from=$theme.yearMarks item=mark}
        <a class="tl-scrub-year" data-page="{$mark.page}" data-year="{$mark.year}"
           href="{g->url params=$theme.pageUrl arg1="page=`$mark.page`"}">{$mark.year}</a>
      {/foreach}
    </nav>
  {/if}
{/if}

{if !empty($theme.navigator)}
  <div class="gbNavigator tl-nav-pages">
    {g->block type="core.Navigator" navigator=$theme.navigator reverseOrder=true}
  </div>
{/if}
