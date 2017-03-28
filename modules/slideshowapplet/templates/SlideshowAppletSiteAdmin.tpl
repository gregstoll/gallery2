{*
 * $Revision: 17730 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="Slideshow Applet Settings"} </h2>
</div>

<input type="hidden" name="{g->formVar var="form[variable][type]"}" />

<table><tr valign="top"><td>

<div class="gbBlock">
  <h3>{g->text text="Defaults"}</h3>
  <p>{g->text text="These variables provide default values for applets users execute on your site. Users will be able to override these defaults by making changes in the user interface of the applets, or by changing their local defaults file."}</p>
{if empty($form.slideshowdefaultVariables)}
  <p>{g->text text="You have no default variables"}</p>
{else}

  <table class="gbDataTable">
    <tr>
      <th> {g->text text="Variable"} </th>
      <th> {g->text text="Action"} </th>
    </tr>
    {foreach from=$form.slideshowdefaultVariables item=variable}
      <tr class="{cycle values="gbEven,gbOdd"}">
	<td>{$variable}</td>
	<td><a href="{g->url arg1="controller=slideshowapplet.SlideshowAppletSiteAdmin"
	   arg2="form[action][delete]=1"
	   arg3=$variable|regex_replace:"/^(.*?)=.*$/":"form[delete][variable]=\\1"
	   arg4="form[variable][type]=default" arg5="mode=variables"}">
	   {g->text text="Delete"}</a></td>
      </tr>
    {/foreach}
  </table>
{/if}
</div>

<div class="gbBlock">
  <h4> {g->text text="Add a new default variable"} </h4>

  {if isset($form.error.default)}
  <div class="giError">
    {g->text text="You must enter a variable name and value"}
  </div>
  {/if}

  {g->text text="New variable"}<br/>
  <input type="text" name="{g->formVar var="form[default][name]"}" /> =
  <input type="text" name="{g->formVar var="form[default][value]"}" />
</div>

<div class="gbBlock gcBackground1">
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][add]"}" value="{g->text text="Add variable"}"
   onclick="javascript:this.form['{g->formVar var="form[variable][type]"}'].value='default';this.form.submit();" />
</div>

<div class="gbBlock">
  <h3>{g->text text="Overrides"}</h3>
  <p>{g->text text="These variables override any other values for applets users execute on your site. Users will not be able to change these values."}</p>
{if empty($form.slideshowoverrideVariables)}
  <p>{g->text text="You have no override variables"}</p>
{else}

  <table class="gbDataTable">
    <tr>
      <th> {g->text text="Variable"} </th>
      <th> {g->text text="Action"} </th>
    </tr>
    {foreach from=$form.slideshowoverrideVariables item=variable}
      <tr class="{cycle values="gbEven,gbOdd"}">
	<td>{$variable}</td>
	<td><a href="{g->url arg1="controller=slideshowapplet.SlideshowAppletSiteAdmin"
	   arg2="form[action][delete]=1"
	   arg3=$variable|regex_replace:"/^(.*?)=.*$/":"form[delete][variable]=\\1"
	   arg4="form[variable][type]=override" arg5="mode=variables"}">
	   {g->text text="Delete"}</a></td>
      </tr>
    {/foreach}
  </table>
{/if}
</div>

<div class="gbBlock">
  <h4> {g->text text="Add a new override variable"} </h4>

  {if isset($form.error.override)}
  <div class="giError">
    {g->text text="You must enter a variable name and value"}
  </div>
  {/if}

  {g->text text="New variable"}<br/>
  <input type="text" name="{g->formVar var="form[override][name]"}" /> =
  <input type="text" name="{g->formVar var="form[override][value]"}" />
</div>

<div class="gbBlock gcBackground1">
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][add]"}" value="{g->text text="Add variable"}"
   onclick="javascript:this.form['{g->formVar var="form[variable][type]"}'].value='override';this.form.submit();" />
</div>


</td><td>
<div class="gbBlock">
  <h3>{g->text text="Help"}</h3>
  <p>{g->text text="Here are a selection of variables that affect slideshows."}</p>
  <table class="gbDataTable">
    <tr><th>{g->text text="variable"}</th><th>{g->text text="values"}</th><th>{g->text text="help"}</th>
    </tr><tr class="gbEven"><td>slideshowMaxPictures</td><td>100</td>
      <td>{g->text text="maximum number of pictures shown in one go"}</td>
    </tr><tr class="gbOdd"><td>slideshowRecursive</td><td>true/false</td>
      <td>{g->text text="does slideshow display pictures inside sub-albums?"}</td>
    </tr><tr class="gbEven"><td>slideshowLowRez</td><td>true/false</td>
      <td>{g->text text="if true, will prevent the slideshow from downloading the full-resolution pictures"}</td>
    </tr><tr class="gbOdd"><td>slideshowLoop</td><td>true/false</td>
      <td>{g->text text="does slideshow loop when it gets to the end?"}</td>
    </tr><tr class="gbEven"><td>slideshowNoStretch</td><td>true/false</td>
      <td>{g->text text="if true, pictures smaller than the screen won't be stretched"}</td>
    </tr><tr class="gbOdd"><td>slideshowPreloadAll</td><td>true/false</td>
      <td>{g->text text="if true, the slideshow will download pictures before they're needed, which can speed up, but also may waste bandwidth"}</td>
    </tr><tr class="gbEven"><td>slideshowColor</td><td>0,0,0</td>
      <td>{g->text text="color of the slideshow background"}</td>
    </tr><tr class="gbOdd"><td>slideshowFontSize</td><td>30</td>
      <td>{g->text text="size of text overlay"}</td>
    </tr><tr class="gbEven"><td>slideshowFontName</td><td>arial</td>
      <td>{g->text text="font of text overlay"}</td>
    </tr><tr class="gbOdd"><td>slideshowRandom</td><td>true/false</td>
      <td>{g->text text="should the pictures be shown in random order?"}</td>
    </tr>
  </table>
  <p><a href="https://gallery.svn.sourceforge.net/svnroot/gallery/trunk/gallery_remote/defaults.properties" target="_blank">
	{g->text text="Complete list of variables"}</a></p>
</div>
</td></tr></table>
