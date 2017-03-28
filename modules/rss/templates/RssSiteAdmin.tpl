{*
 * $Revision: 17380 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2>
    {g->text text="RSS Settings"}
  </h2>
</div>

{if isset($status.saved)}
<div class="gbBlock">
  <h2 class="giSuccess">
    {g->text text="Settings have been saved"}
  </h2>
</div>
{elseif !empty($form.error)}
<div class="gbBlock">
  <h2 class="giError">
    {g->text text="An error occured"}
  </h2>
</div>
{/if}

<div class="gbTabBar">
  {if ($RssSiteAdmin.mode == 'settings')}
    <span class="giSelected o"><span>
      {g->text text="Settings"}
    </span></span>
  {else}
    <span class="o"><span>
      <a href="{g->url arg1="view=core.SiteAdmin" arg2="subView=rss.RssSiteAdmin"
       arg3="mode=settings"}">{g->text text="Settings"}</a>
    </span></span>
  {/if}

  {if ($form.allowSimpleFeed) }
  {if ($RssSiteAdmin.mode == 'simplefeedsettings')}
    <span class="giSelected o"><span>
      {g->text text="Simple Feed Settings"}
    </span></span>
  {else}
    <span class="o"><span>
      <a href="{g->url arg1="view=core.SiteAdmin" arg2="subView=rss.RssSiteAdmin"
       arg3="mode=simplefeedsettings"}">{g->text text="Simple Feed Settings"}</a>
    </span></span>
  {/if}
  {/if}

  {if ($RssSiteAdmin.mode == 'list')}
    <span class="giSelected o"><span>
      {g->text text="List"}
    </span></span>
  {else}
    <span class="o"><span>
      <a href="{g->url arg1="view=core.SiteAdmin" arg2="subView=rss.RssSiteAdmin"
       arg3="mode=list"}">{g->text text="List"}</a>
    </span></span>
  {/if}
</div>

<input type="hidden" name="{g->formVar var="form[mode]"}" value="{$RssSiteAdmin.mode}" />

{if $RssSiteAdmin.mode == 'settings' }

<div class="gbBlock">
  <h3> {g->text text="General settings"} </h3>
  <p class="giDescription">
    {g->text text="These are the default and maximum values that are used by all feeds on your Gallery"}
  </p>

  <table class="gbDataTable">
  <tr>
    <td>{g->text text="RSS version"}</td>
    <td><select name="{g->formVar var="form[defaultVersion]"}">
        {html_options values=$RssSiteAdmin.rssVersionList selected=$form.defaultVersion output=$RssSiteAdmin.rssVersionList}
    </select></td>
    <td>
      {if isset($form.error.defaultVersion)}
      <div class="giError">
      {g->text text="Invalid version"}
      </div>
      {/if}
    </td>
  </tr><tr>
    <td>{g->text text="Default number of items in feeds"}</td>
    <td><input type="text" name="{g->formVar var="form[defaultCount]"}" value="{$form.defaultCount}" /></td>
    <td>
      {if isset($form.error.defaultCount)}
      <div class="giError">
        {g->text text="Invalid default number"}
      </div>
      {/if}
    </td>
  </tr><tr>
    <td>{g->text text="Maximum number of items in feeds"}</td>
    <td><input type="text" name="{g->formVar var="form[maxCount]"}" value="{$form.maxCount}" /></td>
    <td>
      {if isset($form.error.maxCount)}
      <div class="giError">
        {g->text text="Invalid maximum number"}
      </div>
      {/if}
    </td>
  </tr><tr>
    <td>{g->text text="Maximum age for items in feeds"}</td>
    <td><input type="text" name="{g->formVar var="form[maxAge]"}" value="{$form.maxAge}" /></td>
    <td>
      {if isset($form.error.maxAge)}
      <div class="giError">
        {g->text text="Invalid maximum age number"}
      </div>
      {/if}
    </td>
  </tr><tr>
    <td>{g->text text="Default ttl"}</td>
    <td>
      <input type="text" name="{g->formVar var="form[defaultTtl]"}" value="{$form.defaultTtl}" />
    </td>
    <td>
      {if isset($form.error.defaultTtl)}
      <div class="giError">
        {g->text text="Invalid time to live, this must be a number of minutes (0 to disable ttl)"}
      </div>
      {else}
        {g->text text="Time to live: this must be a number of minutes (0 to disable ttl) aggregators should wait between checks of whether the feed has been updated"}
      {/if}
    </td>
  </tr><tr>
    <td>{g->text text="Default copyright"}</td>
    <td>
      <input type="text" name="{g->formVar var="form[defaultCopyright]"}" value="{$form.defaultCopyright}" />
    </td>
    <td>
      {if isset($form.error.defaultCopyright)}
      <div class="giError">
        {g->text text="Invalid string"}
      </div>
      {/if}
    </td>
  </tr></table>
</div>

<div class="gbBlock">
  <h3> {g->text text="Feed Types"} </h3>
  <p class="giDescription">
    {g->text text="Configure which types of feeds are allowed"}
  </p>
  <table><col width="1%"/><col width="1%"/><col width="50%"/><col width="48%"/>
    <tr><td>
      <input type="checkbox" {if $form.allowSimpleFeed}checked="checked" {/if}
        name="{g->formVar var="form[allowSimpleFeed]"}" id="Rss_allowSimpleFeed"/>
    </td><td colspan="2">
      <label for="Rss_allowSimpleFeed">
        {g->text text="Allow Simple RSS Feed to be used"}
      </label>
    </td><td>
    </td></tr>
    <tr><td></td><td colspan="3">
      {g->text text="If you enable this, users will be able to subscribe to a feed for any album or photo they can view."}
      <br/>
      {g->text text="To edit the settings for Simple Feeds, use the corresponding tab at the top of the screen."}
    </td></tr>

    <tr><td>
      <input type="checkbox" {if $form.allowConfigurableFeed}checked="checked" {/if}
        onclick="refresh(this.form)"
        name="{g->formVar var="form[allowConfigurableFeed]"}" id="Rss_allowConfigurableFeed"/>
    </td><td colspan="3">
      <label for="Rss_allowConfigurableFeed">
        {g->text text="Allow configurable RSS feeds to be used"}
      </label>
    </td></tr>
    <tr><td></td><td colspan="3">
      {g->text text="Only album owners will be able to define such feeds. They can set parameters for the type of feed they want."}
      <br/>
      {g->text text="Once a feed is defined, users will be able to subscribe to those feeds if they can view the associated item."}
    </td></tr>
    <tr><td>
    </td><td>
      <input type="checkbox" {if $form.allowPhotos}checked="checked" {/if}
        {if !$form.allowConfigurableFeed} disabled="disabled" {/if}
        name="{g->formVar var="form[allowPhotos]"}" id="Rss_allowPhotos"/>
    </td><td>
      <label for="Rss_allowPhotos">
        {g->text text="Allow RSS feeds of photos inside an album"}
      </label>
    </td><td>
      {g->text text="Fast"}
    </td></tr>
    <tr><td>
    </td><td>
      <input type="checkbox" {if $form.allowAlbums}checked="checked" {/if}
        {if !$form.allowConfigurableFeed} disabled="disabled" {/if}
        name="{g->formVar var="form[allowAlbums]"}" id="Rss_allowAlbums"/>
    </td><td>
      <label for="Rss_allowAlbums">
        {g->text text="Allow RSS feeds of subalbums of an album"}
      </label>
    </td><td>
      {g->text text="Slow"}
    </td></tr>
    <tr><td>
    </td><td>
      <input type="checkbox" {if $form.allowPhotosRecursive}checked="checked" {/if}
        {if !$form.allowConfigurableFeed} disabled="disabled" {/if}
        name="{g->formVar var="form[allowPhotosRecursive]"}" id="Rss_allowPhotosRecursive"/>
    </td><td>
      <label for="Rss_allowPhotosRecursive">
        {g->text text="Allow RSS feeds of photos inside an album and its subalbums"}
      </label>
    </td><td>
      {g->text text="Slower"}
    </td></tr>
    <tr><td>
    </td><td>
      <input type="checkbox" {if $form.allowCommentsPhoto}checked="checked" {/if}
        {if !$form.allowConfigurableFeed} disabled="disabled" {/if}
        name="{g->formVar var="form[allowCommentsPhoto]"}" id="Rss_allowCommentsPhoto"/>
    </td><td>
      <label for="Rss_allowCommentsPhoto">
        {g->text text="Allow RSS feeds of comments of a photo"}
      </label>
    </td><td>
      {g->text text="Fast"}
    </td></tr>
    <tr><td>
    </td><td>
      <input type="checkbox" {if $form.allowCommentsAlbum}checked="checked" {/if}
        {if !$form.allowConfigurableFeed} disabled="disabled" {/if}
        name="{g->formVar var="form[allowCommentsAlbum]"}" id="Rss_allowCommentsAlbum"/>
    </td><td>
      <label for="Rss_allowCommentsAlbum">
        {g->text text="Allow RSS feeds of comments of an album"}
      </label>
    </td><td>
      {g->text text="Fast"}
    </td></tr>
    <tr><td>
    </td><td>
      <input type="checkbox" {if $form.allowCommentsRecursive}checked="checked" {/if}
        {if !$form.allowConfigurableFeed} disabled="disabled" {/if}
        name="{g->formVar var="form[allowCommentsRecursive]"}" id="Rss_allowCommentsRecursive"/>
    </td><td>
      <label for="Rss_allowCommentsRecursive">
        {g->text text="Allow RSS feeds of comments for an album and its subalbums"}
      </label>
    </td><td>
      {g->text text="Fast"}
    </td></tr>
    <tr><td>
    </td><td>
      <input type="checkbox" {if $form.allowPhotosRandom}checked="checked" {/if}
        {if !$form.allowConfigurableFeed} disabled="disabled" {/if}
        name="{g->formVar var="form[allowPhotosRandom]"}" id="Rss_allowPhotosRandom"/>
    </td><td>
      <label for="Rss_allowPhotosRandom">
        {g->text text="Allow RSS feeds of random photos inside an album"}
      </label>
    </td><td>
      {g->text text="Fast"}
    </td></tr>
    <tr><td>
    </td><td>
      <input type="checkbox" {if $form.allowPhotosRandomRecursive}checked="checked" {/if}
        {if !$form.allowConfigurableFeed} disabled="disabled" {/if}
        name="{g->formVar var="form[allowPhotosRandomRecursive]"}" id="Rss_allowPhotosRandomRecursive"/>
    </td><td>
      <label for="Rss_allowPhotosRandomRecursive">
        {g->text text="Allow RSS feeds of random photos inside an album and its subalbums"}
      </label>
    </td><td>
      {g->text text="Slowest"}
    </td></tr>
  </table>
</div>

<div class="gbBlock gcBackground1">
  <input type="submit" name="{g->formVar var="form[action][save]"}" value="{g->text text="Save Settings"}"/>
  <input type="submit" name="{g->formVar var="form[action][reset]"}" value="{g->text text="Reset"}"/>
</div>

{g->addToTrailer}
<script type="text/javascript">
  // <![CDATA[
  function refresh(form) {ldelim}
    enabled = form.elements.namedItem('{g->formVar var="form[allowConfigurableFeed]"}').checked;

    form.elements.namedItem('{g->formVar var="form[allowAlbums]"}').disabled = !enabled;
    form.elements.namedItem('{g->formVar var="form[allowPhotos]"}').disabled = !enabled;
    form.elements.namedItem('{g->formVar var="form[allowPhotosRecursive]"}').disabled = !enabled;
    form.elements.namedItem('{g->formVar var="form[allowCommentsPhoto]"}').disabled = !enabled;
    form.elements.namedItem('{g->formVar var="form[allowCommentsAlbum]"}').disabled = !enabled;
    form.elements.namedItem('{g->formVar var="form[allowCommentsRecursive]"}').disabled = !enabled;
    {rdelim}
  // ]]>
</script>
{/g->addToTrailer}

{elseif $RssSiteAdmin.mode == 'list'}

<div class="gbBlock">
  <h3> {g->text text="Helping users discover RSS feeds"} </h3>
  <p>{g->text text="If you want to help users discover feeds in your Gallery, you can add the RSS block to the themes used in your Gallery."}</p>
  <p>{g->text text="This will have two effects: the block itself, which users can see and pick feeds from (and get a list of all feeds); and hidden page headers that modern web browsers will use to make it easier for the user to subscribe to the feeds."}</p>
</div>

<div class="gbBlock">
  <h3> {g->text text="Global list of RSS feeds"} </h3>
  {if empty($RssSiteAdmin.feeds)}
    <p> {g->text text="No feeds have yet been defined"} </p>
  {else}

    <table class="gbDataTable">
    <tr>
      <th> {g->text text="Feed link"} </th>
      <th> {g->text text="On item"} </th>
      <th> {g->text text="Feed type"} </th>
      <th> {g->text text="Actions"} </th>
    </tr>
    {foreach from=$RssSiteAdmin.feeds item=feed}
	  <tr class="{cycle values="gbEven,gbOdd"}">
	    <td><a href="{g->url arg1="view=rss.Render" arg2="name=`$feed.name`"}">
	      {$feed.name}</a></td>
	    <td><a href="{g->url arg1="view=core.ShowItem" arg2="itemId=`$feed.itemId`"}">
	      {$feed.item->getTitle()}</a></td>
	    <td>{$RssSiteAdmin.types[$feed.params.feedType]}</td>
	    <td>
	      <a href="{g->url arg1="controller=rss.RssSiteAdmin"
	        arg2="form[action][delete]=`$feed.name`"}">
		{g->text text="delete"}</a>
	      <a href="{g->url arg1="view=core.ItemAdmin" arg2="subView=rss.EditFeed"
	        arg3="form[feedName]=`$feed.name`" arg4="itemId=`$feed.itemId`"
		arg5="mode=edit"}">
		{g->text text="edit"}</a>
	    </td>
	  </tr>
    {/foreach}
	</table>
  {/if}
</div>

{else}

<div class="gbBlock">
  <h3> {g->text text="Simple Feed settings"} </h3>
  <p class="giDescription">
    {g->text text="Choose what type of data that will be present in the feed"}
  </p>

  <table class="gbDataTable">

  <tr valign="top">
    <td rowspan="5">{g->text text="For albums"}</td>
    <td>
      <input type="radio" name="{g->formVar var="form[sfAlbumType]"}"
        value="photos" {if $form.sfAlbumType=='photos'}checked="checked" {/if}
	id="RssSiteAdmin_typePhotos" />
    </td><td>
      <label for="RssSiteAdmin_typePhotos">
        {g->text text="Items in the album"}
      </label>
    </td><td>
      {g->text text="Fast"}
    </td>
  </tr><tr valign="top">
    <td>
      <input type="radio" name="{g->formVar var="form[sfAlbumType]"}"
        value="album" {if $form.sfAlbumType=='album'}checked="checked" {/if}
	id="RssSiteAdmin_typeAlbum" />
    </td><td>
      <label for="RssSiteAdmin_typeAlbum">
        {g->text text="Sub-albums of the album"}
      </label>
    </td><td>
      {g->text text="Slow"}
    </td>
  </tr><tr valign="top">
    <td>
      <input type="radio" name="{g->formVar var="form[sfAlbumType]"}"
        value="photosRecursive" {if $form.sfAlbumType=='photosRecursive'}checked="checked" {/if}
	id="RssSiteAdmin_typePhotosRecursive" />
    </td><td>
      <label for="RssSiteAdmin_typePhotosRecursive">
        {g->text text="Items in the album and its subalbums"}
      </label>
    </td><td>
      {g->text text="Slower"}
    </td>
  </tr><tr>
    <td></td>
    <td colspan="2" valign="top">
      <label for="RssSiteAdmin_typePhotosRecursiveLimit">
        {g->text text="Limit the number of items per album"}
      </label>
      <input type="text" size="5" name="{g->formVar var="form[sfPhotosRecursiveLimit]"}"
        {if isset($form.sfPhotosRecursiveLimit)} value="{$form.sfPhotosRecursiveLimit}" {/if}
	id="RssSiteAdmin_typePhotosRecursiveLimit" />
      {if isset($form.error.sfPhotosRecursiveLimit)}
      <div class="giError">
      {g->text text="Invalid limit (must be a positive number, 0 to disable the limit)"}
      </div>
      {else}
      {g->text text="(enter 0 to disable the limit)"}
      {/if}
    </td>
  </tr><tr>
    <td>
      <input type="radio" name="{g->formVar var="form[sfAlbumType]"}"
        value="commentsAlbum" {if $form.sfAlbumType=='commentsAlbum'}checked="checked" {/if}
	id="RssSiteAdmin_typeCommentsAlbum" />
    </td><td>
      <label for="RssSiteAdmin_typeCommentsAlbum">
        {g->text text="Comments for the album"}
      </label>
    </td><td>
      {g->text text="Fast"}
    </td>
  </tr><tr>
  <td></td>
    <td>
      <input type="radio" name="{g->formVar var="form[sfAlbumType]"}"
        value="commentsRecursive" {if $form.sfAlbumType=='commentsRecursive'}checked="checked" {/if}
	id="RssSiteAdmin_typeCommentsRecursive" />
    </td><td>
      <label for="RssSiteAdmin_typeCommentsRecursive">
        {g->text text="Comments for an album and its subalbums"}
      </label>
    </td><td>
      {g->text text="Fast"}
    </td>
  </tr>

  <tr><td>&nbsp;</td></tr>

  <tr valign="top">
    <td>{g->text text="For items"}</td>
    <td><input type="radio" name="fake"
        value="commentsPhoto" checked="checked" id="RssSiteAdmin_typeCommentsPhoto" />
    </td><td>
      <label for="RssSiteAdmin_typeAlbum">
        {g->text text="Comments for items (always selected)"}
      </label>
    </td><td>
      {g->text text="Fast"}
    </td>
  </tr>

  <tr><td>&nbsp;</td></tr>

  <tr>
    <td rowspan="2" valign="top">{g->text text="Which items"}</td>
    <td>
      <input type="radio" name="{g->formVar var="form[sfDate]"}" value="new"
        {if $form.sfDate=='new'}checked="checked"{/if} id="RssSiteAdmin_dateNew" />
    </td><td colspan="2">
      <label for="RssSiteAdmin_dateNew">{g->text text="New items only"}</label>
    </td>
  </tr><tr>
    <td>
      <input type="radio" name="{g->formVar var="form[sfDate]"}" value="updated"
        {if $form.sfDate=='updated'}checked="checked"{/if} id="RssSiteAdmin_dateUpdated" />
    </td><td colspan="2">
      <label for="RssSiteAdmin_dateUpdated">{g->text text="New and updated items"}</label>
    </td>
  </tr>
  </table>

</div>

<div class="gbBlock gcBackground1">
  <input type="submit" name="{g->formVar var="form[action][save]"}" value="{g->text text="Save Settings"}"/>
  <input type="submit" name="{g->formVar var="form[action][reset]"}" value="{g->text text="Reset"}"/>
</div>

{/if}
