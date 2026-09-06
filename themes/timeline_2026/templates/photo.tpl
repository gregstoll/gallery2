{* timeline_2026 - single item view *}
{if !empty($theme.imageViews)}
  {assign var="image" value=$theme.imageViews[$theme.imageViewsIndex]}
{/if}
{assign var="isMovie" value=0}
{if $theme.item.entityType == 'GalleryMovieItem' || !empty($theme.item.duration)}
  {assign var="isMovie" value=1}
{/if}

<div class="tl-item">
  <div class="tl-item-stage">
    {if $isMovie}
      {* Gallery2's stock movie markup is an ActiveX <object> plus an <embed> of
         the raw file, neither of which works in a current browser. *}
      <video controls playsinline preload="metadata"
             {if !empty($theme.item.width)}width="{$theme.item.width}"{/if}
             {if !empty($theme.item.height)}height="{$theme.item.height}"{/if}>
        <source src="media/{$theme.item.id}.mp4" type="video/mp4"/>
        {g->text text="Your browser cannot play this video."}
      </video>
    {elseif !empty($theme.imageViews)}
      <a href="{g->url arg1="view=core.DownloadItem" arg2="itemId=`$theme.item.id`"}">
        {g->image item=$theme.item image=$image maxSize=$theme.params.maxSize}
      </a>
    {else}
      <div class="tl-empty">{g->text text="no image"}</div>
    {/if}
  </div>

  <aside class="tl-info">
    <h1>{if !empty($theme.item.title)}{$theme.item.title|markup}{else}{$theme.item.pathComponent}{/if}</h1>
    {if !empty($theme.item.description)}
      <p class="tl-desc">{$theme.item.description|markup}</p>
    {/if}
    <dl class="tl-facts">
      {if !empty($theme.item.originationTimestamp)}
        <dt>{g->text text="Taken"}</dt>
        <dd>{g->date timestamp=$theme.item.originationTimestamp format="%d %B %Y"}</dd>
      {/if}
      {if !empty($theme.item.width)}
        <dt>{g->text text="Dimensions"}</dt>
        <dd>{$theme.item.width} &times; {$theme.item.height}</dd>
      {/if}
      {if !empty($theme.item.duration)}
        <dt>{g->text text="Duration"}</dt>
        <dd>{$theme.item.duration}s</dd>
      {/if}
    </dl>
    <div class="tl-info-blocks">
      {g->block type="core.ItemInfo" item=$theme.item showDate=true showSize=true class="giInfo"}
      {g->block type="core.PhotoSizes" class="giInfo"}
    </div>
    <p class="tl-item-dl">
      <a href="{g->url arg1="view=core.DownloadItem" arg2="itemId=`$theme.item.id`"}">
        {g->text text="Download the original file"}</a>
    </p>
  </aside>
</div>

{if !empty($theme.navigator)}
  <div class="gbNavigator tl-nav-pages">
    {g->block type="core.Navigator" navigator=$theme.navigator reverseOrder=true}
  </div>
{/if}
