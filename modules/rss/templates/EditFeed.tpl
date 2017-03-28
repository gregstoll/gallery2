{*
 * $Revision: 17380 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="RSS Feeds"} </h2>
</div>

{if !empty($status)}
<div class="gbBlock">
  <h2 class="giSuccess">
  {if isset($status.saved)}
    {g->text text="Settings have been saved"}
  {elseif isset($status.deleted)}  
    {g->text text="Feed successfully deleted"}
  {/if}
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
  {if ($EditFeed.mode == 'list')}
    <span class="giSelected o"><span>
      {g->text text="List"}
    </span></span>
  {else}
    <span class="o"><span>
      <a href="{g->url arg1="view=core.ItemAdmin" arg2="subView=rss.EditFeed"
       arg3="mode=list" arg4="itemId=`$EditFeed.itemId`"}">{g->text text="List"}</a>
    </span></span>
  {/if}

  {if ($EditFeed.mode == 'edit')}
    <span class="giSelected o"><span>
      {g->text text="Edit Feed"}
    </span></span>
  {/if}

  {if ($EditFeed.mode == 'new')}
    <span class="giSelected o"><span>
      {g->text text="New Feed"}
    </span></span>
  {else}
    <span class="o"><span>
      <a href="{g->url arg1="view=core.ItemAdmin" arg2="subView=rss.EditFeed"
       arg3="mode=new" arg4="itemId=`$EditFeed.itemId`"}">{g->text text="New Feed"}</a>
    </span></span>
  {/if}
</div>

<input type="hidden" name="{g->formVar var="form[mode]"}" value="{$EditFeed.mode}" />

{if $EditFeed.mode == 'edit' || $EditFeed.mode == 'new' }

{if isset($EditFeed.feedUrl)}
  <div class="gbBlock">
  <h3> {g->text text="Feed URL"}</h3>
  <p><a href="{$EditFeed.feedUrl}">{$EditFeed.feedUrl}</a></p>
  </div>
{/if}

  <div class="gbBlock">
  <h3> {g->text text="Feed name"} </h3>
{if ($EditFeed.mode == 'edit')}
  <p class="giDescription">
    {g->text text="You can't change the name of the feed"}
  </p>

  <table class="gbDataTable">
  <tr>
    <td>{g->text text="Feed name"}</td>
    <td>{$form.feedName}
      <input type="hidden" name="{g->formVar var="form[feedName]"}" value="{$form.feedName}" />
    </td>
  </tr>
{else}
  <p class="giDescription">
    {g->text text="Pick name for your feed (this name will be referenced in the URL to your feed so try to use only letters)"}
  </p>

  <table class="gbDataTable">
  <tr>
    <td>{g->text text="Feed name"}</td>
    <td>
      <input type="text" name="{g->formVar var="form[feedName]"}"
        {if isset($form.feedName)}value="{$form.feedName}"{/if} />
      {if isset($form.error.feedName)}
      <div class="giError">
        {g->text text="You must enter a name"}
      </div>
      {/if}
      {if isset($form.error.feedNameCollision)}
      <div class="giError">
        {g->text text="This name is already used, please select another one"}
      </div>
      {/if}
    </td>
  </tr>
{/if}
  <tr>
    <td valign="top">{g->text text="Description"}</td>
    <td>
      <textarea rows="4" cols="60" name="{g->formVar var="form[description]"}">
        {if isset($EditFeed.description)}{$EditFeed.description}{/if}
      </textarea>
    </td>
  </tr>
  </table>

  <h3> {g->text text="Feed settings"} </h3>
  <p class="giDescription">
    {g->text text="Choose what type of data that will be present in the feed"}
  </p>

  <input type="hidden" name="{g->formVar var="form[itemId]"}" value="{$EditFeed.itemId}" />
  <input type="hidden" name="{g->formVar var="form[type]"}" value="{$EditFeed.type}" />

  <table class="gbDataTable">

{if $EditFeed.type == 'album'}
  <tr valign="top">
    <td rowspan="7">{g->text text="Type of feed"}</td>
    <td>
      <input type="radio" name="{g->formVar var="form[feedType]"}"
        value="photos" {if $EditFeed.feedType=='photos'}checked="checked" {/if}
        id="EditFeed_typePhotos" />
    </td><td>
      <label for="EditFeed_typePhotos">
        {g->text text="Items in this album"}
      </label>
    {if !$param.allowPhotos}
      <br/>{g->text text="Feeds of this type are disallowed by the administrator. Users won't be able to view them."}
    {/if}
    </td>
  </tr><tr valign="top">
    <td>
      <input type="radio" name="{g->formVar var="form[feedType]"}"
        value="album" {if $EditFeed.feedType=='album'}checked="checked" {/if}
        id="EditFeed_typeAlbum" />
    </td><td>
      <label for="EditFeed_typeAlbum">
        {g->text text="Sub-albums of this album"}
      </label>
    {if !$param.allowAlbums}
      <br/>{g->text text="Feeds of this type are disallowed by the administrator. Users won't be able to view them."}
    {/if}
    </td>
  </tr><tr valign="top">
    <td>
      <input type="radio" name="{g->formVar var="form[feedType]"}"
        value="photosRecursive" {if $EditFeed.feedType=='photosRecursive'}checked="checked" {/if}
        id="EditFeed_typePhotosRecursive" />
    </td><td>
      <label for="EditFeed_typePhotosRecursive">
        {g->text text="Items in this album and its subalbums"}
      </label>
    {if !$param.allowPhotosRecursive}
      <br/>{g->text text="Feeds of this type are disallowed by the administrator. Users won't be able to view them."}
    {/if}
      <br/>
      <label for="EditFeed_typePhotosRecursiveLimit">
        {g->text text="Limit the number of items per album"}
      </label>
      <input type="text" size="5" name="{g->formVar var="form[photosRecursiveLimit]"}"
        {if isset($EditFeed.photosRecursiveLimit)}value="{$EditFeed.photosRecursiveLimit}"{/if}
        id="EditFeed_typePhotosRecursiveLimit" />
      {if isset($form.error.photosRecursiveLimit)}
      <div class="giError">
      {g->text text="Invalid limit (must be a positive number, 0 to disable the limit)"}
      </div>
      {else}
      {g->text text="(enter 0 to disable the limit)"}
      {/if}
    </td>
  </tr><tr valign="top">
    <td>
      <input type="radio" name="{g->formVar var="form[feedType]"}"
        value="commentsAlbum" {if $EditFeed.feedType=='commentsAlbum'}checked="checked"{/if}
        id="EditFeed_typeCommentsAlbum" />
    </td><td>
      <label for="EditFeed_typeCommentsAlbum">
        {g->text text="Comments for this album"}
      </label>
    {if !$param.allowCommentsAlbum}
      <br/>{g->text text="Feeds of this type are disallowed by the administrator. Users won't be able to view them."}
    {/if}
    </td>
  </tr><tr valign="top">
    <td>
      <input type="radio" name="{g->formVar var="form[feedType]"}"
        value="commentsRecursive" {if $EditFeed.feedType=='commentsRecursive'}checked="checked"{/if}
        id="EditFeed_typeCommentsRecursive" />
    </td><td>
      <label for="EditFeed_typeCommentsRecursive">
        {g->text text="Comments for this album and its subalbums"}
      </label>
    {if !$param.allowCommentsRecursive}
      <br/>{g->text text="Feeds of this type are disallowed by the administrator. Users won't be able to view them."}
    {/if}
    </td>
  </tr><tr valign="top">
    <td>
      <input type="radio" name="{g->formVar var="form[feedType]"}"
        value="photosRandom" {if $EditFeed.feedType=='photosRandom'}checked="checked"{/if}
        id="EditFeed_typePhotosRandom" />
    </td><td>
      <label for="EditFeed_typePhotosRandom">
        {g->text text="Random pictures inside this album"}
      </label>
    {if !$param.allowPhotosRandom}
      <br/>{g->text text="Feeds of this type are disallowed by the administrator. Users won't be able to view them."}
    {/if}
    </td>
  </tr><tr valign="top">
    <td>
      <input type="radio" name="{g->formVar var="form[feedType]"}"
        value="photosRandomRecursive" {if $EditFeed.feedType=='photosRandomRecursive'}checked="checked"{/if}
        id="EditFeed_typePhotosRandomRecursive" />
    </td><td>
      <label for="EditFeed_typePhotosRandomRecursive">
        {g->text text="Random pictures inside this album and its subalbums"}
      </label>
    {if !$param.allowPhotosRandomRecursive}
      <br/>{g->text text="Feeds of this type are disallowed by the administrator. Users won't be able to view them."}
    {/if}
    </td>
  </tr>
  {else}
  <tr valign="top">
    <td>{g->text text="Type of feed"}</td>
    <td><input type="radio" name="{g->formVar var="form[feedType]"}"
        value="commentsPhoto" checked="checked" id="EditFeed_typeCommentsPhoto" />
    </td><td>
      <label for="EditFeed_typeAlbum">
        {g->text text="Comments for this item"}
      </label>
    {if !$param.allowCommentsPhoto}
      <br/>{g->text text="Feeds of this type are disallowed by the administrator. Users won't be able to view them."}
    {/if}
    </td>
  </tr>
{/if}

  <tr><td>&nbsp;</td></tr>

  <tr>
    <td rowspan="2" valign="top">{g->text text="Which items"}</td>
    <td>
      <input type="radio" name="{g->formVar var="form[feedDate]"}"
        value="new" {if $EditFeed.feedDate=='new'}checked="checked"{/if} id="EditFeed_dateNew" />
    </td><td colspan="2">
      <label for="EditFeed_dateNew">{g->text text="New items only"}</label>
    </td>
  </tr><tr>
    <td>
      <input type="radio" name="{g->formVar var="form[feedDate]"}" value="updated"
        {if $EditFeed.feedDate=='updated'}checked="checked"{/if} id="EditFeed_dateUpdated" />
    </td><td colspan="2">
      <label for="EditFeed_dateUpdated">{g->text text="New and updated items"}</label>
    </td>
  </tr>
  </table>

  <p class="giDescription">
    {g->text text="Choose the options for the feed"}
  </p>

  <table class="gbDataTable">
  <tr>
    <td>{g->text text="RSS version"}</td>
    <td>
      <select name="{g->formVar var="form[version]"}">
        {html_options values=$EditFeed.rssVersionList selected=$EditFeed.version
	  output=$EditFeed.rssVersionList}
      </select>
    </td>
    <td>
      {if isset($form.error.version)}
      <div class="giError">
        {g->text text="Invalid version"}
      </div>
      {/if}
    </td>
  </tr>

  <tr>
    <td>{g->text text="Number of items"}</td>
    <td>
      <input type="text" name="{g->formVar var="form[count]"}" value="{$EditFeed.count}" />
    </td>
    <td>
      {if isset($form.error.count)}
      <div class="giError">
        {g->text text="Invalid number"}
      </div>
      {/if}
    </td>
  </tr>

  <tr>
    <td>{g->text text="language tag"}</td>
    <td>
      <input type="text" name="{g->formVar var="form[language]"}" value="{$EditFeed.language}" />
    </td>
    <td>
      {if isset($form.error.language)}
      <div class="giError">
        {g->text text="Invalid language value"}
      </div>
      {/if}
      {g->text text="by default: en-us."}
      <a href="http://blogs.law.harvard.edu/tech/stories/storyReader$15">
        {g->text text="List of all possible values"}
      </a>
    </td>
  </tr>

  <tr>
    <td>{g->text text="category tag"}</td>
    <td>
      <input type="text" name="{g->formVar var="form[category]"}" value="{$EditFeed.category}" />
    </td>
  </tr>

  <tr>
    <td>{g->text text="ttl tag"}</td>
    <td>
      <input type="text" name="{g->formVar var="form[ttl]"}" value="{$EditFeed.ttl}" />
    </td>
    <td>
      {if isset($form.error.ttl)}
      <div class="giError">
        {g->text text="Invalid time to live, this must be a number of minutes (0 to disable ttl)"}
      </div>
      {else}
        {g->text text="Time to live: this must be a number of minutes (0 to disable ttl) aggregators should wait between checks of whether the feed has been updated"}
      {/if}
    </td>
  </tr>

  <tr>
    <td>{g->text text="Copyright"}</td>
    <td>
      <input type="text" name="{g->formVar var="form[copyright]"}" value="{$EditFeed.copyright}" />
    </td>
    <td>
      {if isset($form.error.copyright)}
      <div class="giError">
        {g->text text="Invalid copyright"}
      </div>
      {/if}
    </td>
  </tr>

  <tr>
    <td>{g->text text="Use image tag"}</td>
    <td>
      <input type="checkbox"
        {if isset($EditFeed.useImage) && $EditFeed.useImage} checked="checked" {/if}
        name="{g->formVar var="form[useImage]"}"/> {g->text text="Include image tag"}
    </td>
    <td>
      {g->text text="This adds a main image to the feed, corresponding to the thumbnail for this item"}
      {if isset($form.error.useImage)}
      <div class="giError">
        {g->text text="Invalid value"}
      </div>
      {/if}
    </td>
  </tr>

  <tr>
    <td>{g->text text="Use enclosures"}</td>
    <td>
      <input type="checkbox"
        {if isset($EditFeed.useEnclosure) && $EditFeed.useEnclosure} checked="checked" {/if}
        name="{g->formVar var="form[useEnclosure]"}"/> {g->text text="Include enclosure tags"}
    </td>
    <td>
      {g->text text="This adds an enclosure to each image in the feed, that contains the full resolution image"}
      {if isset($form.error.useEnclosure)}
      <div class="giError">
        {g->text text="Invalid value"}
      </div>
      {/if}
    </td>
  </tr>

  <tr>
    <td>{g->text text="Use Media RSS"}</td>
    <td>
      <input type="checkbox"
        {if isset($EditFeed.useMedia) && $EditFeed.useMedia} checked="checked" {/if}
        name="{g->formVar var="form[useMedia]"}"/> {g->text text="Include Media RSS tags"}
    </td>
    <td>
      {g->text text="This adds Media RSS tags to the feed, which allows media-oriented clients to display the feed in a more useful fashion"}
        (<a href="http://search.yahoo.com/mrss">{g->text text="about Media RSS"}</a>)
      {if isset($form.error.useMedia)}
      <div class="giError">
        {g->text text="Invalid value"}
      </div>
      {/if}
    </td>
  </tr>
  </table>

  <h3> {g->text text="Cloud tag (RSS 2.0)"} </h3>
  <p class="giDescription">
    {g->text text="If you want to use the cloud tag, you can set up its properties here"}
    <br />
    {g->text text="Example: "}
    &lt;cloud domain="<b>{g->text text="domain"}</b>"
	      port="<b>{g->text text="port"}</b>"
	      path="<b>{g->text text="path"}</b>"
	      registerProcedure="<b>{g->text text="registerProcedure"}</b>"
	      protocol="<b>{g->text text="protocol"}</b>" /&gt;
  </p>
  <table class="gbDataTable"><tr>
    <td>{g->text text="Cloud tag"}</td>
    <td>
      <input type="checkbox"
        {if isset($EditFeed.useCloud) && $EditFeed.useCloud} checked="checked" {/if}
        name="{g->formVar var="form[useCloud]"}"/> {g->text text="Include the cloud tag"}
    </td>
    <td>
      {if isset($form.error.useCloud)}
      <div class="giError">
        {g->text text="Invalid value"}
      </div>
      {/if}
    </td>
  </tr><tr>
    <td>{g->text text="domain"}</td>
    <td>
      <input type="text" name="{g->formVar var="form[cloudDomain]"}"
	{if isset($EditFeed.cloudDomain)}value="{$EditFeed.cloudDomain}"{/if} />
    </td>
    <td>
      {if isset($form.error.cloudDomain)}
      <div class="giError">
        {g->text text="Invalid string"}
      </div>
      {/if}
    </td>
  </tr><tr>
    <td>{g->text text="port"}</td>
    <td>
      <input type="text" name="{g->formVar var="form[cloudPort]"}"
        {if isset($EditFeed.cloudPort)}
          value="{$EditFeed.cloudPort}"
	{/if} />
    </td>
    <td>
      {if isset($form.error.cloudPort)}
      <div class="giError">
        {g->text text="Invalid port number"}
      </div>
      {/if}
    </td>
  </tr><tr>
    <td>{g->text text="path"}</td>
    <td>
      <input type="text" name="{g->formVar var="form[cloudPath]"}"
        {if isset($EditFeed.cloudPath)}
	  value="{$EditFeed.cloudPath}"
	{/if} />
    </td>
    <td>
      {if isset($form.error.cloudPath)}
      <div class="giError">
        {g->text text="Invalid string"}
      </div>
      {/if}
    </td>
  </tr><tr>
    <td>{g->text text="registerProcedure"}</td>
    <td>
      <input type="text" name="{g->formVar var="form[cloudRegisterProcedure]"}"
        {if isset($EditFeed.cloudRegisterProcedure)}
	  value="{$EditFeed.cloudRegisterProcedure}"
	{/if} />
    </td>
    <td>
      {if isset($form.error.cloudRegiserProcedure)}
      <div class="giError">
        {g->text text="Invalid string"}
      </div>
      {/if}
    </td>
  </tr><tr>
    <td>{g->text text="protocol"}</td>
    <td>
      <input type="text" name="{g->formVar var="form[cloudProtocol]"}"
        {if isset($EditFeed.cloudProtocol)}
          value="{$EditFeed.cloudProtocol}"
	{/if} />
    </td>
    <td>
      {if isset($form.error.cloudProtocol)}
      <div class="giError">
        {g->text text="Invalid string"}
      </div>
      {/if}
    </td>
  </tr></table>
  </div>

<div class="gbBlock gcBackground1">
{if $EditFeed.mode == 'edit'}
  <input type="submit" name="{g->formVar var="form[action][update]"}"
    value="{g->text text="Update Feed"}"/>
{else}
  <input type="submit" name="{g->formVar var="form[action][create]"}"
    value="{g->text text="Create Feed"}"/>
{/if}
  <input type="submit" name="{g->formVar var="form[action][cancel]"}"
    value="{g->text text="Cancel"}"/>
</div>

{else}

<div class="gbBlock">
  <h3> {g->text text="RSS feeds for this item"} </h3>
  {if empty($EditFeed.feeds)}
    <p> {g->text text="No feeds have yet been defined"} </p>
  {else}

    <table class="gbDataTable">
    <tr>
      <th> {g->text text="Feed link"} </th>
      <th> {g->text text="Feed type"} </th>
      <th> {g->text text="Actions"} </th>
    </tr>
    {foreach from=$EditFeed.feeds item=feed}
	  <tr class="{cycle values="gbEven,gbOdd"}">
	    <td><a href="{g->url arg1="view=rss.Render" arg2="name=`$feed.name`"}">
	      {$feed.name}</a></td>
	    <td>{$EditFeed.types[$feed.params.feedType]}</td>
	    <td>
	      <a href="{g->url arg1="controller=rss.EditFeed"
	        arg2="form[action][delete]=`$feed.name`" arg3="itemId=`$feed.itemId`"}">
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

{/if}
