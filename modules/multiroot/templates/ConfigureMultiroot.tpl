{*
 * $Revision: 16871 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="Configure Multiroot"} </h2>
</div>

{if isset($Multiroot.createdUser)}
<div class="gbBlock"><h2 class="giSuccess">
  {g->text text="User created and permissions assigned on target album"}
</h2></div>
{/if}

<div class="gbBlock">
  <p class="giDescription">
    {g->text text="Multiroot allows you to create an alternate URL to view part of the Gallery.  This configuration screen assists in setup and creation of a PHP file for the alternate URL.  This module never activates and is not needed to use multiroot.  There are two options:"}
    <ol style="list-style-type:square; font-size:1.1em">
      <li> {g->text text="Virtual root"} <br/>
	{g->text text="This option defines a default album for this view and adjusts navigation links so that no parent albums above the default are shown.  Both guests and logged in users accessing the alternate URL will see this effect.  However, Gallery permissions are unchanged so modules like imageblock and search, or manually entered URLs, can lead visitors to items outside the default album.  This option is easier to setup as it uses guest permissions already defined for your Gallery."}
      </li>
      <li> {g->text text="Alternate guest user"} <br/>
	{g->text text="This option defines a new guest view with permissions defined for exactly what this view should display.  Guests using the alternate URL cannot see any items outside those granted permission, by browsing or via any modules or URLs.  Logged in users see whatever their permissions allow, whether accessing the normal or alternate URL.  This option provides greater access control over the alternate view, but requires additional permissions setup:"}
	<br/>
	{g->text text="The original/real guest user must have view permission for all items accessible by any alternate guest user.  The form below assists in creation of an alternate guest and will assign view permission for the default album and all subalbums/items.  If these albums are already public and can remain that way, no more setup is required.  Setup for mutually exclusive views is more complicated: the original guest user must have access to items in all views, so first setup those permissions.  Then use the tools here to create each alternate view.  Just use the set of alternate URLs.  Keep the original Gallery URL unpublished as it shows the combined set of items."}
      </li>
    </ol>
  </p>
  <table class="gbDataTable"><tr>
    <td> {g->text text="URI for this Gallery:"} </td>
    <td> {$Multiroot.baseUri} </td>
  </tr><tr>
    <td> {g->text text="URI for new guest view:"} </td>
    <td>
      <input type="text" size="30" name="{g->formVar var="form[viewUri]"}" value="{$form.viewUri}"/>
      {if isset($form.error.viewUri)}
	<div class="giError"> {g->text text="Missing value"} </div>
      {/if}
    </td>
  </tr><tr>
    <td> {g->text text="Root album of new view"} </td>
    <td>
      <select name="{g->formVar var="form[viewRootId]"}">
      {foreach from=$Multiroot.albumTree item=album}
	<option value="{$album.data.id}"{if $album.data.id==$form.viewRootId
	 } selected="selected"{/if}>
	  {"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"|repeat:$album.depth}--
	  {$album.data.title|markup:strip|default:$album.data.pathComponent}
	</option>
      {/foreach}
      </select>
    </td>
  </tr><tr>
    <td style="vertical-align:top;padding-top:0.8em">
      {g->text text="Username of alternate guest user:"}
    </td><td>
      <input type="text" size="30"
	     name="{g->formVar var="form[guestUser]"}" value="{$form.guestUser}"/> <br/>
      {g->text text="New user will be created if given name does not exist."} <br/>
      {g->text text="Leave blank to define a virtual root."}
      {if isset($form.error.guestUser)}
	<div class="giError"> {g->text text="Missing value"} </div>
      {/if}
    </td>
  </tr></table>
</div>

{if isset($form.generate.go)}
<div class="gbBlock">
  <tt>{$form.viewUri}</tt>
  <span id="php-toggle" class="giBlockToggle gcBackground1 gcBorder2"
	style="border-width: 1px" onclick="BlockToggle('php-text', 'php-toggle')">-</span><br/>
  <pre id="php-text" style="border-width:1px; border-style:dotted; padding:4px"
   class="gcBackground2 gcBorder1">&lt;?php
require('{$Multiroot.basePath}embed.php');
$ret = GalleryEmbed::init(
&nbsp;	array('embedUri' =&gt; '{$form.viewUri}',
&nbsp;	      'g2Uri' =&gt; '{$Multiroot.baseUriDir}',
&nbsp;	      'apiVersion' =&gt; array(1, 2)
&nbsp;	));
if ($ret) {ldelim}
&nbsp;   print '&lt;body&gt;' . $ret-&gt;getAsHtml() . '&lt;/body&gt;';
&nbsp;   return;
{rdelim}
&nbsp;
$gallery-&gt;setConfig('login', true);
$gallery-&gt;setConfig('defaultAlbumId', {$form.viewRootId});
{if isset($Multiroot.viewUserId)
}$gallery-&gt;setConfig('anonymousUserId', {$Multiroot.viewUserId});
{else}$gallery-&gt;setConfig('breadcrumbRootId', {$form.viewRootId});
{/if}&nbsp;
GalleryMain();
?&gt;</pre>
  {if isset($Multiroot.htaccess)}
    <tt>{$Multiroot.viewBase}.htaccess</tt>
    <span id="ht-toggle" class="giBlockToggle gcBackground1 gcBorder2"
	  style="border-width: 1px" onclick="BlockToggle('ht-text', 'ht-toggle')">+</span><br/>
  <pre id="ht-text" style="display:none; border-width:1px; border-style:dotted; padding:4px"
   class="gcBackground2 gcBorder1">{$Multiroot.htaccess}</pre>
  {/if}
</div>
{/if}

<div class="gbBlock gcBackground1">
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][generate]"}" value="{g->text text="Generate Files"}"/>
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][reset]"}" value="{g->text text="Reset"}"/>
</div>
