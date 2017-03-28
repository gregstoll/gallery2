{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="URL Rewrite Setup"} </h2>
</div>

{if !empty($status)}
<div class="gbBlock"><h2 class="giSuccess">
  {if isset($status.saved)}
    {g->text text="Successfully saved settings"}
  {/if}
</h2></div>
{/if}

{if !empty($form.error)}
<div class="gbBlock">
   <h2 class="giError"> {g->text text="An error occured while trying to save your settings"} </h3>

  {if isset($SetupRewrite.errors)}
  <div class="giError">
    {foreach from=$SetupRewrite.errors item=errstr}
      {$errstr}<br/>
    {/foreach}
  </div>
  {/if}
</div>
{/if}

{if isset($SetupRewrite.bootstrap)}
{* Don't offer Apache mod_rewrite if we have detected IIS *}
{if $SetupRewrite.server != 'IIS'}
<div class="gbBlock">
  <h2> <a href="{g->url arg1="controller=rewrite.SetupRewrite" arg2="form[parser]=modrewrite" arg3="form[action][save]=1"}">{g->text text="Apache mod_rewrite"}</a> </h2>

  <p class="giDescription">
    {g->text text="The Apache mod_rewrite module is installed on most Apache servers by default. If you are unsure of what method you should choose then select this. Gallery will try to detect if your server supports mod_rewrite."}
  </p>
</div>
{/if}

{* Don't offer Isapi Rewrite if we have detected Apache *}
{if $SetupRewrite.server != 'APACHE'}
<div class="gbBlock">
  <h2> <a href="{g->url arg1="controller=rewrite.SetupRewrite" arg2="form[parser]=isapirewrite" arg3="form[action][save]=1"}">{g->text text="IIS ISAPI_Rewrite"}</a> </h2>

  <p class="giDescription">
    {g->text text="This method allows for short URLs on IIS servers with ISAPI Rewrite installed. Gallery will try to detect if your server supports this method before activating the module."}<br/>
    <ul>
      <li class="giDescription giWarning">{g->text text="A pattern may not begin with a keyword."}</li>
    </ul>
  </p>
</div>
{/if}

<div class="gbBlock">
  <h2> <a href="{g->url arg1="controller=rewrite.SetupRewrite" arg2="form[parser]=pathinfo" arg3="form[action][save]=1"}">{g->text text="PHP Path Info"}</a> </h2>

  <p class="giDescription">
    {g->text text="Using Path Info is supported by most systems. With this method Gallery parses the URL itself during the request."}
    <ul>
      <li class="giDescription giWarning">{g->text text="It is recomended that you don't activate the 'Download Item' URL since it will slow down Gallery."}</li>
      <li class="giDescription giWarning">{g->text text="Block hotlinking is not supported."}</li>
    </ul>
  </p>
</div>

{else}
{if isset($AdminParser.template)}
    {include file="gallery:`$AdminParser.template`"}

    {if isset($AdminParser.action)}
      <div class="gbBlock gcBackground1">
        <input type="submit" class="inputTypeSubmit"
         name="{g->formVar var="form[action][adminParser]"}" value="{g->text text="Save"}"/>
      </div>
    {/if}
  {/if}
  {if isset($TestResults.template)}
    {include file="gallery:`$TestResults.template`"}

    <div class="gbBlock gcBackground1">
    <input type="submit" class="inputTypeSubmit"
     name="{g->formVar var="form[action][back]"}" value="{g->text text="Back"}"/>

    {if isset($TestResults.action)}
      <input type="submit" class="inputTypeSubmit"
       name="{g->formVar var="form[action][testParser]"}" value="{g->text text="Save"}"/>
    {/if}
    {if !$SetupRewrite.needsConfiguration}
      <input type="submit" class="inputTypeSubmit"
       name="{g->formVar var="form[action][done]"}" value="{g->text text="Done"}"/>
    {/if}
    {if isset($TestResults.refresh)}
      <input type="submit" class="inputTypeSubmit"
       name="{g->formVar var="form[action][refresh]"}" value="{g->text text="Test again"}"/>
    {/if}
    </div>
  {/if}
{/if}
