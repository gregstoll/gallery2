{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div id="gsSidebar" class="gcBorder1">

  {* System Links *}
  <div class="gbBlock">
    {g->block type="core.SystemLinks"
	      order="core.SiteAdmin core.YourAccount core.Login core.Logout"
	      othersAt=4}
  </div>

  {* Breadcrumb *}
  {if !empty($theme.parents)}
  <div class="gbBlock">
    <h3> {g->text text="Navigation"} </h3>
    <ul>
    {foreach from=$theme.parents item=parent}
      <li>
	&raquo;
	<a href="{g->url params=$parent.urlParams}">
	  {$parent.title|markup:strip|default:$parent.pathComponent}
	</a>
      </li>
    {/foreach}
    </ul>
  </div>
  {/if}

  {* Display and slideshow controls *}
  <div class="gbBlock">
    <h3> {g->text text="Display Options"} </h3>
    <ul>
      <li>
	<a id="dtl_link" href="" onclick="album_detailsonoff();this.blur();return false">
	  {g->text text="Hide Details"}
	</a>
      </li>
      <li>
	<a id="lnk_link" href="" onclick="album_itemlinksonoff();this.blur();return false">
	  {g->text text="Show Item Links"}
	</a>
      </li>
      <li>
	<a href="javascript:alert(keyboard_help)">
	  {g->text text="Keyboard Controls"}
	</a>
      </li>
      {if $user.isRegisteredUser}
      <li>
	{g->text text="View"}:&nbsp;
	{if ($theme.guestPreviewMode)}
	<a href="{g->url arg1="controller=core.ShowItem" arg2="guestPreviewMode=0"
			 arg3="return=1"}">{$user.userName}</a> | {g->text text="guest"}
	{else}
	{$user.userName} | <a href="{g->url arg1="controller=core.ShowItem"
			    arg2="guestPreviewMode=1" arg3="return=1"}">{g->text text="guest"}</a>
	{/if}
      </li>
      {/if}
    </ul>
    <ul style="margin-top: 4px">
      <li>
	<strong>{g->text text="Slideshow Options"}:</strong>
      </li>
      <li>
	{g->text text="Delay"}:&nbsp;
	<select id="slide_delay" onchange="slide_setdelay(this.value)">
	 <option value="3">{g->text text="3 seconds"}</option>
	 <option selected="selected" value="5">{g->text text="5 seconds"}</option>
	 <option value="7">{g->text text="7 seconds"}</option>
	 <option value="10">{g->text text="10 seconds"}</option>
	 <option value="15">{g->text text="15 seconds"}</option>
	 <option value="20">{g->text text="20 seconds"}</option>
	</select>
	<br/>
	{g->text text="Direction"}:&nbsp;
	<select id="slide_order" onchange="slide_setorder(this.value)">
	 <option selected="selected" value="1">{g->text text="forward"}</option>
	 <option value="-1">{g->text text="reverse"}</option>
	 <option value="0">{g->text text="random"}</option>
	</select>
      </li>
    </ul>
  </div>

  {* Show the sidebar blocks chosen for this theme *}
  {foreach from=$theme.params.sidebarBlocks item=block}
    {g->block type=$block.0 params=$block.1 class="gbBlock"}
  {/foreach}

  {* Our emergency edit link, if the user removes all blocks containing edit links *}
  {g->block type="core.EmergencyEditItemLink" class="gbBlock" checkBlocks="sidebar,album"}
</div>
