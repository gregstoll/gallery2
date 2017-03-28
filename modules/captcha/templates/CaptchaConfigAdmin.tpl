{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div id="gbBlock gcBackground1">
  <h2 class="giTitle">
    {g->text text="Captcha plugin configuration test"}
  </h2>
</div>

<div class="gbBlock">
  <div class="giDescription">
    {g->text text="The Captcha module requires your webserver to have the GD graphics module installed.  The following GD functions are required."}
  </div>

  <table class="gbDataTable">
    <tr>
      <th>
        {g->text text="Function name"}
      </th>
      <th>
        {g->text text="Pass/fail"}
      </th>
    </tr>
    
    {foreach from=$CaptchaConfigAdmin.gdReport.success item=func}
    <tr class="{cycle values="gbEven,gbOdd"}">
      <td>
        {$func}
      </td>
      <td>
        <div class="giSuccess">
          {g->text text="Available"}
        </div>
      </td>
    </tr>
    {/foreach}

    {foreach from=$CaptchaConfigAdmin.gdReport.fail item=func}
    <tr class="{cycle values="gbEven,gbOdd"}">
      <td>
        {$func}
      </td>
      <td>
        <div class="giError">
          {g->text text="Missing"}
        </div>
      </td>
    </tr>
    {/foreach}
  </table>

  {if !empty($CaptchaConfigAdmin.gdReport.fail)}
  <div class="giError">
    {g->text text="Critical GD functions are not available.   Please ask your system administrator for help."}
  </div>
  {else}
  <div class="giSuccess">
    {g->text text="Your webserver is properly configured to use the Captcha module."}
  </div>
  {/if}
</div>

<div class="gbBlock gcBackground1">
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][continue]"}" value="{g->text text="Continue"}"/>
</div>
