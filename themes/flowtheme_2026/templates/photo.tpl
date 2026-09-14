{* flowtheme_2026 - single item view *}
{if !empty($theme.imageViews)}
  {assign var="image" value=$theme.imageViews[$theme.imageViewsIndex]}
{/if}
{assign var="isMovie" value=0}
{if $theme.item.entityType == 'GalleryMovieItem' || !empty($theme.item.duration)}
  {assign var="isMovie" value=1}
{/if}
<div class="ft-layout{if !empty($theme.params.sidebarBlocks)} has-side{/if}">
  {if !empty($theme.params.sidebarBlocks)}
  <aside class="ft-side">{g->theme include="sidebar.tpl"}</aside>
  {/if}
  <div class="ft-content">

    <div class="ft-pagehead">
      {if !empty($theme.item.title)}
        <h1>{$theme.item.title|markup}</h1>
      {/if}
      {if !empty($theme.item.description)}
        <p class="ft-desc">{$theme.item.description|markup}</p>
      {/if}
      <div class="ft-meta">
        {g->block type="core.ItemInfo" item=$theme.item showDate=true
                  showOwner=$theme.params.showImageOwner class="giInfo"}
        {g->block type="core.PhotoSizes" class="giInfo"}
      </div>
    </div>

    <div class="ft-photo">
      {if $isMovie}
        {* Gallery2's own movie markup is a 2003-era ActiveX <object> plus an
           <embed> of the raw AVI - neither works in a current browser. Serve
           the H.264/AAC transcode instead, made by transcode-movies.sh. *}
        <div class="ft-photo-frame ft-video">
          <video controls playsinline preload="metadata"
                 {if !empty($theme.item.width)}width="{$theme.item.width}"{/if}
                 {if !empty($theme.item.height)}height="{$theme.item.height}"{/if}>
            <source src="media/{$theme.item.id}.mp4" type="video/mp4"/>
            {g->text text="Your browser cannot play this video."}
          </video>
          <p class="ft-video-note">
            <a href="{g->url arg1="view=core.DownloadItem" arg2="itemId=`$theme.item.id`"}">
              {g->text text="Download the original file"}</a>
            {if !empty($theme.item.duration)} &middot; {$theme.item.duration}s{/if}
          </p>
        </div>
      {elseif !empty($theme.imageViews)}
        <div class="ft-photo-frame">
          <a href="{g->url arg1="view=core.DownloadItem" arg2="itemId=`$theme.item.id`"}">
            {g->image item=$theme.item image=$image maxSize=$theme.params.maxSize}
          </a>
        </div>
      {else}
        <div class="ft-empty">{g->text text="no image"}</div>
      {/if}
    </div>

    {if !empty($theme.navigator)}
      <div class="ft-photo-nav">
        {g->block type="core.Navigator" navigator=$theme.navigator reverseOrder=true}
      </div>
    {/if}
  </div>
</div>
