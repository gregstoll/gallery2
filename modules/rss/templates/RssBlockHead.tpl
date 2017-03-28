{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
{g->callback type="rss.FeedList"}
{if !empty($block.rss.feeds)}
  {foreach from=$block.rss.feeds item=feed}
    <link rel="alternate" type="application/rss+xml" title="{$feed}"
      href="{g->url arg1="view=rss.Render" arg2="name=`$feed`"}" />
  {/foreach}
{/if}

