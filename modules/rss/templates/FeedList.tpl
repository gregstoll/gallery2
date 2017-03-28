{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="RSS Feeds"} </h2>
</div>

<div class="gbBlock">
  <p class="giDescription">
    {g->text text="This is a list of all the feeds that are available on this Gallery"}
  </p>

  {if empty($FeedList.feeds)}
    <p> {g->text text="No feeds have yet been defined"} </p>
  {else}

    <table class="gbDataTable">
    <tr>
      <th> {g->text text="Feed link"} </th>
      <th> {g->text text="Description"} </th>
    </tr>
    {foreach from=$FeedList.feeds item=feed}
      <tr class="{cycle values="gbEven,gbOdd"}">
	<td><a href="{g->url arg1="view=rss.Render" arg2="name=`$feed.name`"}">
	  {$feed.name}</a></td>
	<td>{if isset($feed.params.description)}{$feed.params.description}{/if}</td>
      </tr>
    {/foreach}
	</table>
  {/if}
</div>